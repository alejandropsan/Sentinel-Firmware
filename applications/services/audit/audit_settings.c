#include "audit_storage.h"

#include <flipper_format/flipper_format.h>

void audit_settings_load(Storage* storage, const char* path, AuditSettings* settings) {
    furi_check(storage && path && settings);
    settings->min_level = AuditLevelInfo;
    settings->max_file_kb = AUDIT_DEFAULT_MAX_FILE_SIZE / 1024;
    settings->max_files = AUDIT_DEFAULT_MAX_FILES;

    FlipperFormat* file = flipper_format_file_alloc(storage);
    FuriString* text = furi_string_alloc();
    uint32_t value;

    if(flipper_format_file_open_existing(file, path) &&
       flipper_format_read_header(file, text, &value) &&
       furi_string_equal_str(text, AUDIT_SETTINGS_TYPE) && value == AUDIT_SETTINGS_VERSION) {
        // Rewind before each key so missing or reordered keys do not matter
        if(flipper_format_rewind(file) && flipper_format_read_string(file, "min_level", text)) {
            AuditLevel level = audit_level_from_name(furi_string_get_cstr(text));
            if(level < AuditLevelCount) settings->min_level = level;
        }
        if(flipper_format_rewind(file) &&
           flipper_format_read_uint32(file, "max_file_kb", &value, 1) &&
           value >= AUDIT_MIN_FILE_KB && value <= AUDIT_MAX_FILE_KB) {
            settings->max_file_kb = value;
        }
        if(flipper_format_rewind(file) &&
           flipper_format_read_uint32(file, "max_files", &value, 1) && value >= 1 &&
           value <= AUDIT_MAX_FILES) {
            settings->max_files = value;
        }
    }

    furi_string_free(text);
    flipper_format_free(file);
}

bool audit_settings_save(Storage* storage, const char* path, const AuditSettings* settings) {
    furi_check(storage && path && settings);

    FlipperFormat* file = flipper_format_file_alloc(storage);
    uint32_t max_file_kb = settings->max_file_kb;
    uint32_t max_files = settings->max_files;
    bool ok =
        flipper_format_file_open_always(file, path) &&
        flipper_format_write_header_cstr(file, AUDIT_SETTINGS_TYPE, AUDIT_SETTINGS_VERSION) &&
        flipper_format_write_comment_cstr(
            file, "min_level: debug, info, warning, critical, off") &&
        flipper_format_write_string_cstr(
            file, "min_level", audit_level_name(settings->min_level)) &&
        flipper_format_write_comment_cstr(
            file, "Applied at boot: max_file_kb 4-1024, max_files 1-10") &&
        flipper_format_write_uint32(file, "max_file_kb", &max_file_kb, 1) &&
        flipper_format_write_uint32(file, "max_files", &max_files, 1);
    flipper_format_free(file);
    return ok;
}
