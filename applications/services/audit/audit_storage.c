#include "audit_storage.h"

#include <furi_hal_crypto.h>

#define TAG "AuditStorage"

#define AUDIT_LINE_MAX  200 /**< Upper bound of a formatted line, used for rotation */
#define AUDIT_TAIL_READ 512 /**< Bytes read from the end of a file to restore the chain */
#define AUDIT_MAC_KEY   FURI_HAL_CRYPTO_ENCLAVE_UNIQUE_KEY_SLOT
#define AUDIT_MAC_MAX   (AUDIT_CHAIN_LEN + AUDIT_LINE_MAX + 4)

static const char* const audit_level_names[AuditLevelCount] = {
    "debug",
    "info",
    "warning",
    "critical",
};

struct AuditStorage {
    Storage* storage;
    File* file;
    FuriString* dir;
    FuriString* path;
    FuriString* line;
    uint32_t max_file_size;
    uint8_t max_files;
    uint32_t file_size;
    bool file_open;
    bool needs_newline;
    AuditChain chain;
};

void audit_chain_reset(AuditChain* chain) {
    furi_check(chain);
    chain->seq = 0;
    memset(chain->chain, '0', AUDIT_CHAIN_LEN);
    chain->chain[AUDIT_CHAIN_LEN] = '\0';
}

void audit_sanitize(char* text) {
    furi_check(text);
    for(; *text; text++) {
        if(*text == ',' || *text == '"' || *text < ' ' || *text > '~') *text = '_';
    }
}

const char* audit_level_name(AuditLevel level) {
    return level < AuditLevelCount ? audit_level_names[level] : "unknown";
}

/**
 * Chain value: CBC-MAC with the device unique key (hardware AES) over
 * len(32-bit BE) || previous_chain || line, zero padded to the block size.
 * The length prefix makes CBC-MAC safe for variable-length messages.
 * Writes 16 hex characters, or 16 'x' if the key is unavailable.
 */
static void
    audit_chain_compute(char* out, const char* previous, const char* data, size_t data_len) {
    uint8_t buffer[AUDIT_MAC_MAX + 16] = {0};
    size_t message_len = AUDIT_CHAIN_LEN + data_len;
    furi_check(4 + message_len <= AUDIT_MAC_MAX);

    buffer[0] = message_len >> 24;
    buffer[1] = message_len >> 16;
    buffer[2] = message_len >> 8;
    buffer[3] = message_len;
    memcpy(&buffer[4], previous, AUDIT_CHAIN_LEN);
    memcpy(&buffer[4 + AUDIT_CHAIN_LEN], data, data_len);
    size_t padded_len = (4 + message_len + 15) & ~(size_t)15;

    const uint8_t iv[16] = {0};
    bool ok = false;
    if(furi_hal_crypto_enclave_ensure_key(AUDIT_MAC_KEY) &&
       furi_hal_crypto_enclave_load_key(AUDIT_MAC_KEY, iv)) {
        // In-place CBC encryption: the last block is the MAC
        ok = furi_hal_crypto_encrypt(buffer, buffer, padded_len);
        furi_hal_crypto_enclave_unload_key(AUDIT_MAC_KEY);
    }

    if(ok) {
        const uint8_t* mac = &buffer[padded_len - 16];
        for(size_t i = 0; i < AUDIT_CHAIN_LEN / 2; i++) {
            snprintf(&out[i * 2], 3, "%02x", mac[i]);
        }
    } else {
        FURI_LOG_E(TAG, "MAC unavailable");
        memset(out, 'x', AUDIT_CHAIN_LEN);
        out[AUDIT_CHAIN_LEN] = '\0';
    }
}

void audit_format_line(
    FuriString* line,
    AuditChain* chain,
    const DateTime* datetime,
    uint32_t uptime_ms,
    AuditLevel level,
    const char* event,
    const char* detail) {
    furi_check(line && chain && datetime && event && detail);

    chain->seq++;
    furi_string_printf(
        line,
        "%lu,%04u-%02u-%02uT%02u:%02u:%02u,%lu,%s,%s,%s",
        chain->seq,
        datetime->year,
        datetime->month,
        datetime->day,
        datetime->hour,
        datetime->minute,
        datetime->second,
        uptime_ms,
        audit_level_name(level),
        event,
        detail);

    char next[AUDIT_CHAIN_LEN + 1];
    audit_chain_compute(next, chain->chain, furi_string_get_cstr(line), furi_string_size(line));
    memcpy(chain->chain, next, sizeof(next));
    furi_string_cat_printf(line, ",%s\n", chain->chain);
}

bool audit_parse_line(const char* line, AuditChain* chain) {
    furi_check(line && chain);
    if(line[0] < '0' || line[0] > '9') return false; // header, comment or empty

    const char* last_comma = strrchr(line, ',');
    if(!last_comma) return false;
    const char* value = last_comma + 1;
    size_t len = strcspn(value, "\r\n");
    if(len != AUDIT_CHAIN_LEN) return false;

    chain->seq = strtoul(line, NULL, 10);
    memcpy(chain->chain, value, AUDIT_CHAIN_LEN);
    chain->chain[AUDIT_CHAIN_LEN] = '\0';
    return true;
}

void audit_file_path(FuriString* path, const char* dir, uint8_t index) {
    furi_check(path && dir);
    if(index == 0) {
        furi_string_printf(path, "%s/audit.csv", dir);
    } else {
        furi_string_printf(path, "%s/audit.%u.csv", dir, index);
    }
}

AuditStorage* audit_storage_alloc(
    Storage* storage,
    const char* dir,
    uint32_t max_file_size,
    uint8_t max_files) {
    furi_check(storage && dir);
    furi_check(max_files >= 1);

    AuditStorage* instance = malloc(sizeof(AuditStorage));
    instance->storage = storage;
    instance->file = storage_file_alloc(storage);
    instance->dir = furi_string_alloc_set(dir);
    instance->path = furi_string_alloc();
    instance->line = furi_string_alloc();
    instance->max_file_size = max_file_size;
    instance->max_files = max_files;
    audit_chain_reset(&instance->chain);
    return instance;
}

void audit_storage_free(AuditStorage* instance) {
    furi_check(instance);
    audit_storage_end(instance);
    storage_file_free(instance->file);
    furi_string_free(instance->dir);
    furi_string_free(instance->path);
    furi_string_free(instance->line);
    free(instance);
}

/** Create every directory component of the log directory below /ext. */
static bool audit_storage_mkdir(AuditStorage* instance) {
    const char* dir = furi_string_get_cstr(instance->dir);
    size_t prefix = strlen(STORAGE_EXT_PATH_PREFIX) + 1;
    bool ok = true;
    for(size_t i = prefix; ok; i++) {
        if(dir[i] == '/' || dir[i] == '\0') {
            furi_string_set_strn(instance->path, dir, i);
            ok = storage_simply_mkdir(instance->storage, furi_string_get_cstr(instance->path));
            if(dir[i] == '\0') break;
        }
    }
    return ok;
}

/** Restore the chain from the last complete data line of a file. */
static bool audit_storage_restore(AuditStorage* instance, bool is_current) {
    File* file = instance->file;
    if(!storage_file_open(
           file, furi_string_get_cstr(instance->path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        storage_file_close(file);
        return false;
    }

    bool found = false;
    uint64_t size = storage_file_size(file);
    uint32_t offset = size > AUDIT_TAIL_READ ? size - AUDIT_TAIL_READ : 0;
    char* buffer = malloc(AUDIT_TAIL_READ + 1);

    if(storage_file_seek(file, offset, true)) {
        size_t read = storage_file_read(file, buffer, AUDIT_TAIL_READ);
        buffer[read] = '\0';
        // A write interrupted by power loss leaves an unterminated line
        if(is_current) instance->needs_newline = read > 0 && buffer[read - 1] != '\n';

        char* end = strrchr(buffer, '\n');
        if(end) *end = '\0'; // drop the unterminated tail, if any
        while(end && !found) {
            char* start = strrchr(buffer, '\n');
            if(!start && offset > 0) break; // first chunk line may be partial
            found = audit_parse_line(start ? start + 1 : buffer, &instance->chain);
            if(!start) break;
            *start = '\0';
        }
    }

    free(buffer);
    storage_file_close(file);
    return found;
}

bool audit_storage_open(AuditStorage* instance) {
    furi_check(instance);
    audit_storage_end(instance);
    audit_chain_reset(&instance->chain);
    instance->needs_newline = false;

    if(!audit_storage_mkdir(instance)) {
        FURI_LOG_E(TAG, "Failed to create %s", furi_string_get_cstr(instance->dir));
        return false;
    }

    for(uint8_t i = 0; i < instance->max_files; i++) {
        audit_file_path(instance->path, furi_string_get_cstr(instance->dir), i);
        if(audit_storage_restore(instance, i == 0)) break;
    }
    FURI_LOG_I(TAG, "Restored seq %lu", instance->chain.seq);
    return true;
}

static bool audit_storage_put(AuditStorage* instance, const char* data, size_t size) {
    if(storage_file_write(instance->file, data, size) != size) return false;
    instance->file_size += size;
    return true;
}

bool audit_storage_begin(AuditStorage* instance) {
    furi_check(instance);
    if(instance->file_open) return true;

    audit_file_path(instance->path, furi_string_get_cstr(instance->dir), 0);
    if(!storage_file_open(
           instance->file, furi_string_get_cstr(instance->path), FSAM_WRITE, FSOM_OPEN_APPEND)) {
        storage_file_close(instance->file);
        FURI_LOG_E(TAG, "Failed to open %s", furi_string_get_cstr(instance->path));
        return false;
    }
    instance->file_open = true;
    instance->file_size = storage_file_size(instance->file);

    bool ok = true;
    if(instance->file_size == 0) {
        ok = audit_storage_put(instance, AUDIT_HEADER, strlen(AUDIT_HEADER));
    } else if(instance->needs_newline) {
        ok = audit_storage_put(instance, "\n", 1);
    }
    instance->needs_newline = false;

    if(!ok) audit_storage_end(instance);
    return ok;
}

void audit_storage_end(AuditStorage* instance) {
    furi_check(instance);
    if(!instance->file_open) return;
    storage_file_close(instance->file);
    instance->file_open = false;
}

/** Shift audit.csv -> audit.1.csv -> ... and drop the oldest file. */
static bool audit_storage_rotate(AuditStorage* instance) {
    const char* dir = furi_string_get_cstr(instance->dir);
    FuriString* older = furi_string_alloc();

    audit_storage_end(instance);
    for(uint8_t i = instance->max_files - 1; i > 0; i--) {
        audit_file_path(older, dir, i);
        audit_file_path(instance->path, dir, i - 1);
        storage_common_remove(instance->storage, furi_string_get_cstr(older));
        storage_common_rename(
            instance->storage, furi_string_get_cstr(instance->path), furi_string_get_cstr(older));
    }
    if(instance->max_files == 1) {
        audit_file_path(instance->path, dir, 0);
        storage_common_remove(instance->storage, furi_string_get_cstr(instance->path));
    }

    furi_string_free(older);
    return audit_storage_begin(instance);
}

static bool audit_storage_write_line(
    AuditStorage* instance,
    const DateTime* datetime,
    uint32_t uptime_ms,
    AuditLevel level,
    const char* event,
    const char* detail) {
    AuditChain previous = instance->chain;
    audit_format_line(instance->line, &instance->chain, datetime, uptime_ms, level, event, detail);
    if(audit_storage_put(
           instance, furi_string_get_cstr(instance->line), furi_string_size(instance->line))) {
        return true;
    }
    instance->chain = previous;
    return false;
}

bool audit_storage_write(
    AuditStorage* instance,
    const DateTime* datetime,
    uint32_t uptime_ms,
    AuditLevel level,
    const char* event,
    const char* detail) {
    furi_check(instance);
    if(!instance->file_open) return false;

    if(instance->file_size > strlen(AUDIT_HEADER) &&
       instance->file_size + AUDIT_LINE_MAX > instance->max_file_size) {
        if(!audit_storage_rotate(instance)) return false;
        char files[12];
        snprintf(files, sizeof(files), "files=%u", instance->max_files);
        if(!audit_storage_write_line(
               instance, datetime, uptime_ms, AuditLevelInfo, "audit.rotate", files)) {
            return false;
        }
    }

    return audit_storage_write_line(instance, datetime, uptime_ms, level, event, detail);
}

const AuditChain* audit_storage_get_chain(AuditStorage* instance) {
    furi_check(instance);
    return &instance->chain;
}
