/**
 * @file audit_cli.c
 * @brief `audit` CLI command (plugin loaded from the SD card).
 *
 * tail, export and verify read the log files directly. level and clear go
 * through the audit service API.
 */
#include "audit_storage.h"

#include <furi.h>
#include <storage/storage.h>
#include <toolbox/args.h>
#include <toolbox/pipe.h>
#include <toolbox/cli/cli_command.h>

#define AUDIT_CLI_APPID     "cli"
#define AUDIT_TAIL_DEFAULT  20
#define AUDIT_TAIL_MAX      100
#define AUDIT_VERIFY_REPORT 20 /**< Problems printed in detail */

typedef void (*AuditCliLineCallback)(FuriString* line, void* context);

static bool audit_cli_is_data_line(FuriString* line) {
    return furi_string_size(line) > 0 && !furi_string_start_with_str(line, "#") &&
           !furi_string_start_with_str(line, "seq,");
}

/** Call `callback` for every data line, oldest file first. Returns false if interrupted. */
static bool audit_cli_for_each_line(PipeSide* pipe, AuditCliLineCallback callback, void* context) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    FuriString* path = furi_string_alloc();
    FuriString* line = furi_string_alloc();
    char buffer[64];
    bool completed = true;

    for(int8_t index = AUDIT_MAX_FILES - 1; index >= 0 && completed; index--) {
        audit_file_path(path, AUDIT_DIR, index);
        if(!storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            storage_file_close(file);
            continue;
        }

        furi_string_reset(line);
        size_t read;
        while((read = storage_file_read(file, buffer, sizeof(buffer))) > 0) {
            for(size_t i = 0; i < read; i++) {
                if(buffer[i] != '\n') {
                    furi_string_push_back(line, buffer[i]);
                    continue;
                }
                if(audit_cli_is_data_line(line)) callback(line, context);
                furi_string_reset(line);
            }
            if(cli_is_pipe_broken_or_is_etx_next_char(pipe)) {
                completed = false;
                break;
            }
        }
        // An unterminated last line (interrupted write) is still shown
        if(audit_cli_is_data_line(line)) callback(line, context);
        storage_file_close(file);
    }

    furi_string_free(line);
    furi_string_free(path);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    return completed;
}

static void audit_cli_print_line(FuriString* line, void* context) {
    UNUSED(context);
    printf("%s\r\n", furi_string_get_cstr(line));
}

typedef struct {
    FuriString** lines;
    size_t capacity;
    size_t count; /**< Total lines seen */
} AuditCliTail;

static void audit_cli_tail_line(FuriString* line, void* context) {
    AuditCliTail* tail = context;
    furi_string_set(tail->lines[tail->count % tail->capacity], line);
    tail->count++;
}

static void audit_cli_tail(PipeSide* pipe, FuriString* args) {
    int count = AUDIT_TAIL_DEFAULT;
    if(args_length(args) && (!args_read_int_and_trim(args, &count) || count < 1)) {
        printf("Usage: audit tail [1-%d]\r\n", AUDIT_TAIL_MAX);
        return;
    }
    if(count > AUDIT_TAIL_MAX) count = AUDIT_TAIL_MAX;

    AuditCliTail tail = {
        .lines = malloc(sizeof(FuriString*) * count),
        .capacity = count,
        .count = 0,
    };
    for(size_t i = 0; i < tail.capacity; i++) {
        tail.lines[i] = furi_string_alloc();
    }

    if(audit_cli_for_each_line(pipe, audit_cli_tail_line, &tail)) {
        size_t shown = MIN(tail.count, tail.capacity);
        if(!shown) printf("Audit log is empty\r\n");
        for(size_t i = tail.count - shown; i < tail.count; i++) {
            printf("%s\r\n", furi_string_get_cstr(tail.lines[i % tail.capacity]));
        }
    }

    for(size_t i = 0; i < tail.capacity; i++) {
        furi_string_free(tail.lines[i]);
    }
    free(tail.lines);
}

static void audit_cli_export(PipeSide* pipe) {
    // Single header for the whole export, regardless of how many files there are
    printf("%s", AUDIT_HEADER);
    if(!audit_cli_for_each_line(pipe, audit_cli_print_line, NULL)) {
        printf("Interrupted\r\n");
    }
}

typedef struct {
    AuditChain chain;
    bool started;
    uint32_t first_seq;
    uint32_t checked;
    uint32_t problems;
} AuditCliVerify;

static void audit_cli_verify_problem(AuditCliVerify* verify, const char* format, ...) {
    verify->problems++;
    if(verify->problems > AUDIT_VERIFY_REPORT) return;
    // vprintf is not exported to plugins
    FuriString* text = furi_string_alloc();
    va_list args;
    va_start(args, format);
    furi_string_vprintf(text, format, args);
    va_end(args);
    printf("%s\r\n", furi_string_get_cstr(text));
    furi_string_free(text);
}

static void audit_cli_verify_line(FuriString* line, void* context) {
    AuditCliVerify* verify = context;
    const char* text = furi_string_get_cstr(line);
    verify->checked++;

    if(!verify->started) {
        AuditChain first;
        if(!audit_parse_line(text, &first)) {
            audit_cli_verify_problem(verify, "malformed line: %.40s", text);
            return;
        }
        verify->started = true;
        verify->first_seq = first.seq;
        if(first.seq != 1) {
            // Older files were rotated out: this line is the starting point
            verify->chain = first;
            return;
        }
        audit_chain_reset(&verify->chain);
    }

    uint32_t expected_seq = verify->chain.seq + 1;
    switch(audit_verify_line(text, &verify->chain)) {
    case AuditVerifyOk:
        break;
    case AuditVerifyMalformed:
        audit_cli_verify_problem(verify, "malformed or incomplete line: %.40s", text);
        break;
    case AuditVerifySeqGap:
        audit_cli_verify_problem(
            verify,
            "seq %lu: expected %lu (lines missing or reordered)",
            verify->chain.seq,
            expected_seq);
        break;
    case AuditVerifyMacMismatch:
        audit_cli_verify_problem(
            verify,
            "seq %lu: MAC mismatch (line modified, or key unavailable when written)",
            verify->chain.seq);
        break;
    }
}

static void audit_cli_verify(PipeSide* pipe) {
    AuditCliVerify verify = {0};
    if(!audit_cli_for_each_line(pipe, audit_cli_verify_line, &verify)) {
        printf("Interrupted\r\n");
        return;
    }
    if(!verify.checked) {
        printf("Audit log is empty\r\n");
        return;
    }
    if(verify.problems > AUDIT_VERIFY_REPORT) {
        printf("... and %lu more problems\r\n", verify.problems - AUDIT_VERIFY_REPORT);
    }
    printf(
        "Checked %lu events (seq %lu-%lu)\r\n", verify.checked, verify.first_seq, verify.chain.seq);
    if(verify.first_seq > 1) {
        printf(
            "Note: events before seq %lu are not on the SD card (rotated out or deleted), "
            "so seq %lu is the starting point and cannot be checked\r\n",
            verify.first_seq,
            verify.first_seq);
    }
    if(verify.problems) {
        printf("Result: FAILED (%lu problems)\r\n", verify.problems);
    } else {
        printf("Result: OK\r\n");
    }
}

static void audit_cli_level(FuriString* args) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    AuditSettings settings;
    audit_settings_load(storage, AUDIT_SETTINGS_PATH, &settings);
    Audit* audit = furi_record_exists(RECORD_AUDIT) ? furi_record_open(RECORD_AUDIT) : NULL;

    FuriString* name = furi_string_alloc();
    if(!args_read_string_and_trim(args, name)) {
        AuditLevel current = audit ? audit_get_level(audit) : settings.min_level;
        printf("Level: %s\r\n", audit_level_name(current));
        if(current != settings.min_level) {
            printf(
                "Saved level: %s (applied at next boot)\r\n",
                audit_level_name(settings.min_level));
        }
    } else {
        AuditLevel level = audit_level_from_name(furi_string_get_cstr(name));
        if(level >= AuditLevelCount) {
            printf("Unknown level. Use: debug, info, warning, critical or off\r\n");
        } else {
            settings.min_level = level;
            // Save first, so the live level never differs from the file because of an SD error
            storage_simply_mkdir(storage, EXT_PATH(".sentinel"));
            storage_simply_mkdir(storage, AUDIT_DIR);
            if(!audit_settings_save(storage, AUDIT_SETTINGS_PATH, &settings)) {
                printf("Failed to save %s\r\n", AUDIT_SETTINGS_PATH);
            } else {
                if(audit) audit_set_level(audit, level);
                printf("Level set to %s\r\n", audit_level_name(level));
            }
        }
    }

    furi_string_free(name);
    if(audit) furi_record_close(RECORD_AUDIT);
    furi_record_close(RECORD_STORAGE);
}

static void audit_cli_clear(FuriString* args) {
    FuriString* confirm = furi_string_alloc();
    if(!args_read_string_and_trim(args, confirm) || !furi_string_equal_str(confirm, "confirm")) {
        printf("This deletes the whole audit log. Run 'audit clear confirm' to do it.\r\n");
    } else if(!furi_record_exists(RECORD_AUDIT)) {
        printf("Audit service is not running\r\n");
    } else {
        Audit* audit = furi_record_open(RECORD_AUDIT);
        if(audit_clear(audit)) {
            printf("Audit log cleared\r\n");
        } else {
            printf("Failed to clear the audit log (SD card missing or file in use)\r\n");
        }
        furi_record_close(RECORD_AUDIT);
    }
    furi_string_free(confirm);
}

static void audit_cli_usage(void) {
    printf("Usage:\r\n");
    printf("audit tail [n]\t - show the last n events (default %d)\r\n", AUDIT_TAIL_DEFAULT);
    printf("audit export\t - print the whole log as CSV, oldest first\r\n");
    printf("audit verify\t - check the MAC chain (only on the device that wrote it)\r\n");
    printf(
        "audit level [l]\t - show or set the minimum level: debug, info, warning, critical, off\r\n");
    printf("audit clear\t - delete the whole log (seq and chain continue)\r\n");
}

static void audit_cli_execute(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(context);
    FuriString* command = furi_string_alloc();

    if(!args_read_string_and_trim(args, command)) {
        audit_cli_usage();
    } else if(furi_string_equal_str(command, "tail")) {
        audit_cli_tail(pipe, args);
    } else if(furi_string_equal_str(command, "export")) {
        audit_cli_export(pipe);
    } else if(furi_string_equal_str(command, "verify")) {
        audit_cli_verify(pipe);
    } else if(furi_string_equal_str(command, "level")) {
        audit_cli_level(args);
    } else if(furi_string_equal_str(command, "clear")) {
        audit_cli_clear(args);
    } else {
        audit_cli_usage();
    }

    furi_string_free(command);
}

CLI_COMMAND_INTERFACE(audit, audit_cli_execute, CliCommandFlagParallelSafe, 2048, AUDIT_CLI_APPID);
