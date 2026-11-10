#include "audit.h"
#include "audit_storage.h"

#include <furi.h>
#include <furi_hal_rtc.h>
#include <toolbox/version.h>
#include <loader/loader.h>
#include <desktop/desktop.h>
#include <toolbox/api_lock.h>

#define TAG "AuditSrv"

#define AUDIT_QUEUE_SIZE   16
#define AUDIT_PENDING_SIZE 16 /**< Events kept in RAM while the SD card is unavailable */

#define AUDIT_FLASH_START 0x08000000UL
#define AUDIT_FLASH_END   0x08100000UL

typedef enum {
    AuditMessageTypeEvent,
    AuditMessageTypeStorage, /**< SD card mounted or unmounted */
    AuditMessageTypeLoader, /**< Loader event, `arg` is the LoaderEventType */
    AuditMessageTypeConfig, /**< Like Event, but written whatever the minimum level */
    AuditMessageTypeClear, /**< audit_clear(), `arg` points to an AuditClearRequest */
} AuditMessageType;

typedef struct {
    FuriApiLock lock;
    bool result;
} AuditClearRequest;

typedef struct {
    AuditMessageType type;
    AuditLevel level;
    DateTime datetime;
    uint32_t uptime_ms;
    uint32_t arg;
    char event[AUDIT_EVENT_SIZE];
    char detail[AUDIT_DETAIL_SIZE];
} AuditMessage;

struct Audit {
    FuriMessageQueue* queue;
    uint32_t dropped; /**< Accessed atomically */
    uint32_t min_level; /**< AuditLevel, accessed atomically */
};

static void audit_drop(Audit* audit) {
    __atomic_fetch_add(&audit->dropped, 1, __ATOMIC_RELAXED);
}

static void audit_message_init(
    AuditMessage* message,
    AuditMessageType type,
    AuditLevel level,
    const char* event) {
    memset(message, 0, sizeof(AuditMessage));
    message->type = type;
    message->level = level;
    message->uptime_ms = furi_get_tick();
    furi_hal_rtc_get_datetime(&message->datetime);
    if(event) strlcpy(message->event, event, sizeof(message->event));
}

static void audit_queue_va(
    Audit* audit,
    AuditMessageType type,
    AuditLevel level,
    const char* event,
    const char* format,
    va_list args) {
    AuditMessage message;
    audit_message_init(&message, type, level, event);
    vsnprintf(message.detail, sizeof(message.detail), format, args);
    if(furi_message_queue_put(audit->queue, &message, 0) != FuriStatusOk) audit_drop(audit);
}

static void audit_queue_config(Audit* audit, const char* event, const char* format, ...) {
    va_list args;
    va_start(args, format);
    audit_queue_va(audit, AuditMessageTypeConfig, AuditLevelWarning, event, format, args);
    va_end(args);
}

AuditLevel audit_get_level(Audit* audit) {
    furi_check(audit);
    return __atomic_load_n(&audit->min_level, __ATOMIC_RELAXED);
}

void audit_log_va(
    Audit* audit,
    AuditLevel level,
    const char* event,
    const char* format,
    va_list args) {
    furi_check(audit && event && format);
    furi_check(level < AuditLevelOff);
    // Checked again when written, once the settings are loaded from the SD card
    if(level < audit_get_level(audit)) return;
    audit_queue_va(audit, AuditMessageTypeEvent, level, event, format, args);
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

void audit_set_level(Audit* audit, AuditLevel level) {
    furi_check(audit && level < AuditLevelCount);
    AuditLevel previous = __atomic_exchange_n(&audit->min_level, level, __ATOMIC_RELAXED);
    audit_queue_config(
        audit,
        "audit.config",
        "level=%s from=%s",
        audit_level_name(level),
        audit_level_name(previous));
}

bool audit_clear(Audit* audit) {
    furi_check(audit);
    AuditClearRequest request = {.lock = api_lock_alloc_locked()};
    AuditMessage message = {
        .type = AuditMessageTypeClear,
        .arg = (uintptr_t)&request, // pointers are 32-bit on this target
    };
    furi_check(furi_message_queue_put(audit->queue, &message, FuriWaitForever) == FuriStatusOk);
    api_lock_wait_unlock_and_free(request.lock);
    return request.result;
}

static void audit_storage_event_callback(const void* message, void* context) {
    const StorageEvent* event = message;
    Audit* audit = context;
    if(event->type == StorageEventTypeCardMount || event->type == StorageEventTypeCardUnmount ||
       event->type == StorageEventTypeCardMountError) {
        // Written once a card is available again, with the time it happened
        if(event->type == StorageEventTypeCardMount) {
            audit_log(audit, AuditLevelInfo, "sd.mount", "%s", "");
        } else if(event->type == StorageEventTypeCardUnmount) {
            audit_log(audit, AuditLevelInfo, "sd.unmount", "%s", "");
        } else {
            audit_log(audit, AuditLevelWarning, "sd.error", "%s", "");
        }
        AuditMessage notify = {.type = AuditMessageTypeStorage};
        // If the queue is full the SD state is re-checked on the next event anyway
        furi_message_queue_put(audit->queue, &notify, 0);
    }
}

static void audit_desktop_event_callback(const void* message, void* context) {
    const DesktopStatus* status = message;
    audit_log(
        context, AuditLevelInfo, status->locked ? "desktop.lock" : "desktop.unlock", "%s", "");
}

/**
 * Runs in the loader thread, so the loader API must not be called here: the
 * app name is looked up later by the audit thread.
 */
static void audit_loader_event_callback(const void* message, void* context) {
    const LoaderEvent* event = message;
    Audit* audit = context;
    if(event->type == LoaderEventTypeNoMoreAppsInQueue) return;

    AuditMessage notify = {
        .type = AuditMessageTypeLoader,
        .level = AuditLevelInfo,
        .uptime_ms = furi_get_tick(),
        .arg = event->type,
    };
    furi_hal_rtc_get_datetime(&notify.datetime);
    if(furi_message_queue_put(audit->queue, &notify, 0) != FuriStatusOk) audit_drop(audit);
}

/**
 * Turn a loader notification into an app event. `last_app` remembers the name
 * of the running app, because it is gone by the time it has stopped.
 * Returns false when there is nothing to log.
 */
static bool audit_loader_message(
    AuditMessage* message,
    Loader* loader,
    FuriString* name,
    char* last_app,
    size_t last_app_size) {
    const char* event = "app.stop";
    const char* app = last_app;
    if(message->arg == LoaderEventTypeApplicationBeforeLoad) {
        // The loader answers after it has started the app, so its name is available.
        // If not, the load failed (reported separately) or the app already exited.
        if(!loader_get_application_name(loader, name)) {
            strlcpy(last_app, "unknown", last_app_size);
            return false;
        }
        strlcpy(last_app, furi_string_get_cstr(name), last_app_size);
        event = "app.start";
    } else if(message->arg == LoaderEventTypeApplicationLoadFailed) {
        message->level = AuditLevelWarning;
        event = "app.load_failed";
        app = "";
    }

    message->type = AuditMessageTypeEvent;
    strlcpy(message->event, event, sizeof(message->event));
    snprintf(message->detail, sizeof(message->detail), "name=%s", app);
    return true;
}

/** Last crash message, if it points into firmware flash. */
static const char* audit_fault_message(void) {
    uint32_t fault = furi_hal_rtc_get_fault_data();
    if(!fault) return "none";
    if(fault < AUDIT_FLASH_START || fault >= AUDIT_FLASH_END) return "unknown";
    return (const char*)fault;
}

/**
 * Load audit.settings and create the log writer. Called once, the first time
 * the SD card is available.
 */
static AuditStorage* audit_storage_start(
    Audit* audit,
    Storage* storage,
    AuditMessage* pending,
    size_t* pending_count) {
    AuditSettings settings;
    audit_settings_load(storage, AUDIT_SETTINGS_PATH, &settings);
    __atomic_store_n(&audit->min_level, settings.min_level, __ATOMIC_RELAXED);

    if(*pending_count < AUDIT_PENDING_SIZE) {
        AuditMessage* message = &pending[(*pending_count)++];
        audit_message_init(message, AuditMessageTypeConfig, AuditLevelInfo, "audit.start");
        snprintf(
            message->detail,
            sizeof(message->detail),
            "level=%s max_kb=%lu files=%u",
            audit_level_name(settings.min_level),
            settings.max_file_kb,
            settings.max_files);
    }

    return audit_storage_alloc(
        storage, AUDIT_DIR, settings.max_file_kb * 1024, settings.max_files);
}

static void
    audit_flush(Audit* audit, AuditStorage* storage, AuditMessage* pending, size_t* pending_count) {
    if(!audit_storage_begin(storage)) return; // keep pending, retry on next event

    AuditLevel min_level = audit_get_level(audit);
    for(size_t i = 0; i < *pending_count; i++) {
        AuditMessage* message = &pending[i];
        if(message->type != AuditMessageTypeConfig && message->level < min_level) continue;
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

static void audit_clear_files(
    Audit* audit,
    AuditStorage* storage,
    bool opened,
    AuditClearRequest* request,
    AuditMessage* pending,
    size_t* pending_count) {
    uint8_t removed = 0;
    request->result = opened && audit_storage_clear(storage, &removed);

    // First line of the new file, chained to the last deleted event
    if(opened && *pending_count < AUDIT_PENDING_SIZE) {
        AuditMessage* message = &pending[(*pending_count)++];
        audit_message_init(message, AuditMessageTypeConfig, AuditLevelWarning, "audit.cleared");
        snprintf(
            message->detail,
            sizeof(message->detail),
            "files=%u%s",
            removed,
            request->result ? "" : " error");
        audit_flush(audit, storage, pending, pending_count);
    }
    api_lock_unlock(request->lock);
}

int32_t audit_srv(void* p) {
    UNUSED(p);

    Audit* audit = malloc(sizeof(Audit));
    audit->queue = furi_message_queue_alloc(AUDIT_QUEUE_SIZE, sizeof(AuditMessage));
    audit->min_level = AuditLevelInfo; // until audit.settings is read
    AuditMessage* pending = malloc(sizeof(AuditMessage) * AUDIT_PENDING_SIZE);
    size_t pending_count = 0;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    furi_pubsub_subscribe(storage_get_pubsub(storage), audit_storage_event_callback, audit);
    AuditStorage* audit_storage = NULL;

    furi_record_create(RECORD_AUDIT, audit);
    audit_log(
        audit,
        AuditLevelInfo,
        "system.boot",
        "fw=%s fault=%.40s",
        version_get_version(NULL),
        audit_fault_message());

    // These services start after this one: wait for them (events stay queued meanwhile)
    Loader* loader = furi_record_open(RECORD_LOADER);
    furi_pubsub_subscribe(loader_get_pubsub(loader), audit_loader_event_callback, audit);
    Desktop* desktop = furi_record_open(RECORD_DESKTOP);
    furi_pubsub_subscribe(
        desktop_api_get_status_pubsub(desktop), audit_desktop_event_callback, audit);
    FuriString* app_name = furi_string_alloc();
    char last_app[32] = "unknown";

    bool opened = false;
    AuditMessage message;
    AuditClearRequest* clear = NULL;

    while(true) {
        furi_check(
            furi_message_queue_get(audit->queue, &message, FuriWaitForever) == FuriStatusOk);

        // Collect everything already queued so it is written in one batch
        do {
            if(message.type == AuditMessageTypeStorage) {
                opened = false; // card changed: restore the chain again
                continue;
            }
            if(message.type == AuditMessageTypeClear) {
                // Later events are collected on the next round, after the clear
                clear = (AuditClearRequest*)(uintptr_t)message.arg;
                break;
            }
            if(message.type == AuditMessageTypeLoader &&
               !audit_loader_message(&message, loader, app_name, last_app, sizeof(last_app))) {
                continue;
            }
            if(pending_count < AUDIT_PENDING_SIZE) {
                pending[pending_count++] = message;
            } else {
                audit_drop(audit);
            }
        } while(furi_message_queue_get(audit->queue, &message, 0) == FuriStatusOk);

        if(!pending_count && !clear) continue;
        if(!opened && storage_sd_status(storage) == FSE_OK) {
            if(!audit_storage) {
                audit_storage = audit_storage_start(audit, storage, pending, &pending_count);
            }
            opened = audit_storage_open(audit_storage);
        }
        if(opened) audit_flush(audit, audit_storage, pending, &pending_count);
        if(clear) {
            audit_clear_files(audit, audit_storage, opened, clear, pending, &pending_count);
            clear = NULL;
        }
    }

    return 0;
}
