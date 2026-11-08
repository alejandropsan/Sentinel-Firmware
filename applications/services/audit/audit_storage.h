/**
 * @file audit_storage.h
 * @brief Audit log file format, hash chain and rotation (firmware internal).
 *
 * Line format: `seq,time,uptime_ms,level,event,detail,chain\n`
 * where chain = first 8 bytes (16 hex chars) of
 * SHA-256(previous_chain || line_without_chain).
 */
#pragma once

#include "audit.h"

#include <furi.h>
#include <storage/storage.h>
#include <datetime/datetime.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIT_DIR                   EXT_PATH(".sentinel/audit")
#define AUDIT_DEFAULT_MAX_FILE_SIZE (64 * 1024)
#define AUDIT_DEFAULT_MAX_FILES     4

#define AUDIT_EVENT_SIZE  24 /**< Event identifier buffer, including terminator */
#define AUDIT_DETAIL_SIZE 97 /**< Detail buffer, including terminator */
#define AUDIT_CHAIN_LEN   16 /**< Hex characters of the chain field */

#define AUDIT_HEADER        \
    "# sentinel-audit v1\n" \
    "seq,time,uptime_ms,level,event,detail,chain\n"

/** Position in the hash chain: last written sequence number and its chain value. */
typedef struct {
    uint32_t seq;
    char chain[AUDIT_CHAIN_LEN + 1];
} AuditChain;

/** @brief Reset to the start of a new log (seq 0, chain of zeros). */
void audit_chain_reset(AuditChain* chain);

/**
 * @brief Make text safe for a CSV field, in place.
 *
 * Replaces commas, quotes and non-printable characters (including line breaks) with '_'.
 */
void audit_sanitize(char* text);

/** @brief Lowercase name of a level ("debug", "info", "warning", "critical"). */
const char* audit_level_name(AuditLevel level);

/**
 * @brief Format one log line (ending in '\n') and advance the chain.
 *
 * `event` and `detail` must already be sanitized.
 */
void audit_format_line(
    FuriString* line,
    AuditChain* chain,
    const DateTime* datetime,
    uint32_t uptime_ms,
    AuditLevel level,
    const char* event,
    const char* detail);

/**
 * @brief Read seq and chain from a data line.
 *
 * @return false for header lines and malformed lines
 */
bool audit_parse_line(const char* line, AuditChain* chain);

/** @brief Path of log file `index` in `dir` (0 = current file, N = N-th older). */
void audit_file_path(FuriString* path, const char* dir, uint8_t index);

typedef struct AuditStorage AuditStorage;

/** @brief Allocate a log writer for `dir`. Does not touch the SD card. */
AuditStorage* audit_storage_alloc(
    Storage* storage,
    const char* dir,
    uint32_t max_file_size,
    uint8_t max_files);

/** @brief Free a log writer. Closes the current file if open. */
void audit_storage_free(AuditStorage* instance);

/**
 * @brief Create the log directory and restore the chain from existing files.
 *
 * Must succeed before audit_storage_begin(). Call again after the SD card was
 * replaced.
 */
bool audit_storage_open(AuditStorage* instance);

/** @brief Open the current log file for appending a batch of events. */
bool audit_storage_begin(AuditStorage* instance);

/**
 * @brief Append one event, rotating files when the size limit is reached.
 *
 * Requires audit_storage_begin(). A rotation writes an `audit.rotate` event
 * at the start of the new file.
 */
bool audit_storage_write(
    AuditStorage* instance,
    const DateTime* datetime,
    uint32_t uptime_ms,
    AuditLevel level,
    const char* event,
    const char* detail);

/** @brief Close the current log file. */
void audit_storage_end(AuditStorage* instance);

/** @brief Current position in the chain. */
const AuditChain* audit_storage_get_chain(AuditStorage* instance);

#ifdef __cplusplus
}
#endif
