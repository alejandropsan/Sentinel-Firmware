/**
 * @file audit_cli.c
 * @brief `audit` CLI command (plugin loaded from the SD card).
 *
 * Reads the log files directly; it does not talk to the audit service.
 */
#include "audit_storage.h"

#include <furi.h>
#include <storage/storage.h>
#include <toolbox/args.h>
#include <toolbox/pipe.h>
#include <toolbox/cli/cli_command.h>

#define AUDIT_CLI_APPID     "cli"
#define AUDIT_CLI_MAX_FILES 10 /**< Scan audit.9.csv ... audit.csv */
#define AUDIT_TAIL_DEFAULT  20
#define AUDIT_TAIL_MAX      100

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

    for(int8_t index = AUDIT_CLI_MAX_FILES - 1; index >= 0 && completed; index--) {
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

static void audit_cli_usage(void) {
    printf("Usage:\r\n");
    printf("audit tail [n]\t - show the last n events (default %d)\r\n", AUDIT_TAIL_DEFAULT);
    printf("audit export\t - print the whole log as CSV, oldest first\r\n");
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
    } else {
        audit_cli_usage();
    }

    furi_string_free(command);
}

CLI_COMMAND_INTERFACE(audit, audit_cli_execute, CliCommandFlagParallelSafe, 2048, AUDIT_CLI_APPID);
