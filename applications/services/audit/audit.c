#include "audit.h"
#include "audit_storage.h"

#include <furi.h>
#include <furi_hal_rtc.h>
#include <toolbox/version.h>

#define TAG "AuditSrv"

#define AUDIT_QUEUE_SIZE   16
#define AUDIT_PENDING_SIZE 16 /**< Events kept in RAM while the SD card is unavailable */
#define AUDIT_MIN_LEVEL    AuditLevelInfo

#define AUDIT_FLASH_START 0x08000000UL
#define AUDIT_FLASH_END   0x08100000UL

typedef enum {
    AuditMessageTypeEvent,
    AuditMessageTypeStorage, /**< SD card mounted or unmounted */
} AuditMessageType;

typedef struct {
    AuditMessageType type;
    AuditLevel level;
    DateTime datetime;
    uint32_t uptime_ms;
    char event[AUDIT_EVENT_SIZE];
    char detail[AUDIT_DETAIL_SIZE];
} AuditMessage;

struct Audit {
    FuriMessageQueue* queue;
    uint32_t dropped; /**< Accessed atomically */
};

static void audit_drop(Audit* audit) {
    __atomic_fetch_add(&audit->dropped, 1, __ATOMIC_RELAXED);
}

void audit_log_va(
    Audit* audit,
    AuditLevel level,
    const char* event,
    const char* format,
    va_list args) {
    furi_check(audit && event && format);
    if(level < AUDIT_MIN_LEVEL) return;

    AuditMessage message = {
        .type = AuditMessageTypeEvent,
        .level = level,
        .uptime_ms = furi_get_tick(),
    };
    furi_hal_rtc_get_datetime(&message.datetime);
    strlcpy(message.event, event, sizeof(message.event));
    vsnprintf(message.detail, sizeof(message.detail), format, args);

    if(furi_message_queue_put(audit->queue, &message, 0) != FuriStatusOk) audit_drop(audit);
}

void audit_log(Audit* audit, AuditLevel level, const char* event, const char* format, ...) {
    va_list args;
    va_start(args, format);
    audit_log_va(audit, level, event, format, args);
    va_end(args);
}

void sentinel_audit(AuditLevel level, const char* event, const char* format, ...) {
    if(!furi_record_exists(RECORD_AUDIT)) return;
    Audit* audit = furi_record_open(RECORD_AUDIT);
    va_list args;
    va_start(args, format);
    audit_log_va(audit, level, event, format, args);
    va_end(args);
    furi_record_close(RECORD_AUDIT);
}

static void audit_storage_event_callback(const void* message, void* context) {
    const StorageEvent* event = message;
    Audit* audit = context;
    if(event->type == StorageEventTypeCardMount || event->type == StorageEventTypeCardUnmount ||
       event->type == StorageEventTypeCardMountError) {
        AuditMessage notify = {.type = AuditMessageTypeStorage};
        // If the queue is full the SD state is re-checked on the next event anyway
        furi_message_queue_put(audit->queue, &notify, 0);
    }
}

/** Last crash message, if it points into firmware flash. */
static const char* audit_fault_message(void) {
    uint32_t fault = furi_hal_rtc_get_fault_data();
    if(!fault) return "none";
    if(fault < AUDIT_FLASH_START || fault >= AUDIT_FLASH_END) return "unknown";
    return (const char*)fault;
}

static void audit_log_start(Audit* audit) {
    audit_log(
        audit,
        AuditLevelInfo,
        "system.boot",
        "fw=%s fault=%.40s",
        version_get_version(NULL),
        audit_fault_message());
    audit_log(
        audit,
        AuditLevelInfo,
        "audit.start",
        "level=%s max_kb=%u files=%u",
        audit_level_name(AUDIT_MIN_LEVEL),
        AUDIT_DEFAULT_MAX_FILE_SIZE / 1024,
        AUDIT_DEFAULT_MAX_FILES);
}

static void
    audit_flush(Audit* audit, AuditStorage* storage, AuditMessage* pending, size_t* pending_count) {
    if(!audit_storage_begin(storage)) return; // keep pending, retry on next event

    for(size_t i = 0; i < *pending_count; i++) {
        AuditMessage* message = &pending[i];
        audit_sanitize(message->event);
        audit_sanitize(message->detail);
        audit_storage_write(
            storage,
            &message->datetime,
            message->uptime_ms,
            message->level,
            message->event,
            message->detail);
    }
    *pending_count = 0;

    uint32_t dropped = __atomic_exchange_n(&audit->dropped, 0, __ATOMIC_RELAXED);
    if(dropped) {
        DateTime datetime;
        furi_hal_rtc_get_datetime(&datetime);
        char detail[24];
        snprintf(detail, sizeof(detail), "count=%lu", dropped);
        audit_storage_write(
            storage, &datetime, furi_get_tick(), AuditLevelWarning, "audit.dropped", detail);
    }

    audit_storage_end(storage);
}

int32_t audit_srv(void* p) {
    UNUSED(p);

    Audit* audit = malloc(sizeof(Audit));
    audit->queue = furi_message_queue_alloc(AUDIT_QUEUE_SIZE, sizeof(AuditMessage));
    AuditMessage* pending = malloc(sizeof(AuditMessage) * AUDIT_PENDING_SIZE);
    size_t pending_count = 0;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    furi_pubsub_subscribe(storage_get_pubsub(storage), audit_storage_event_callback, audit);
    AuditStorage* audit_storage = audit_storage_alloc(
        storage, AUDIT_DIR, AUDIT_DEFAULT_MAX_FILE_SIZE, AUDIT_DEFAULT_MAX_FILES);

    furi_record_create(RECORD_AUDIT, audit);
    audit_log_start(audit);

    bool opened = false;
    AuditMessage message;

    while(true) {
        furi_check(
            furi_message_queue_get(audit->queue, &message, FuriWaitForever) == FuriStatusOk);

        // Collect everything already queued so it is written in one batch
        do {
            if(message.type == AuditMessageTypeStorage) {
                opened = false; // card changed: restore the chain again
            } else if(pending_count < AUDIT_PENDING_SIZE) {
                pending[pending_count++] = message;
            } else {
                audit_drop(audit);
            }
        } while(furi_message_queue_get(audit->queue, &message, 0) == FuriStatusOk);

        if(!pending_count) continue;
        if(!opened) {
            opened = storage_sd_status(storage) == FSE_OK && audit_storage_open(audit_storage);
        }
        if(opened) audit_flush(audit, audit_storage, pending, &pending_count);
    }

    return 0;
}
