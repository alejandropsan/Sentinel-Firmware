/**
 * @file audit.h
 * @brief Sentinel audit log service.
 *
 * Records security-relevant events to the SD card. Logging never blocks the
 * caller: events are queued and written by the service thread. If the queue is
 * full the event is counted as dropped and an `audit.dropped` event is written
 * later.
 *
 * See documentation/sentinel/audit-log-design.md.
 */
#pragma once

#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RECORD_AUDIT "audit"

/** Event severity. Events below the configured minimum level are discarded. */
typedef enum {
    AuditLevelDebug,
    AuditLevelInfo,
    AuditLevelWarning,
    AuditLevelCritical,
    AuditLevelOff, /**< Only valid as minimum level: log nothing but configuration events */
    AuditLevelCount,
} AuditLevel;

typedef struct Audit Audit;

/**
 * @brief Queue an audit event.
 *
 * Safe to call from any thread (not from ISR). Never blocks.
 *
 * @param audit  Audit instance (from furi_record_open(RECORD_AUDIT))
 * @param level  Event severity
 * @param event  Event identifier, e.g. "pin.fail" (truncated to 23 characters)
 * @param format printf-style format for the detail field (truncated to 96 characters)
 */
void audit_log(Audit* audit, AuditLevel level, const char* event, const char* format, ...)
    __attribute__((__format__(__printf__, 4, 5)));

/** @brief va_list variant of audit_log(). */
void audit_log_va(
    Audit* audit,
    AuditLevel level,
    const char* event,
    const char* format,
    va_list args);

/**
 * @brief Queue an audit event if the audit service is running.
 *
 * Convenience wrapper for code outside the service (desktop, rpc, ...). Does
 * nothing when the audit service is not part of the firmware or not started.
 */
void sentinel_audit(AuditLevel level, const char* event, const char* format, ...)
    __attribute__((__format__(__printf__, 3, 4)));

/** @brief Current minimum level. */
AuditLevel audit_get_level(Audit* audit);

/**
 * @brief Change the minimum level until the next reboot.
 *
 * Logs an `audit.config` event, which is written whatever the level. Saving
 * the setting to audit.settings is up to the caller.
 */
void audit_set_level(Audit* audit, AuditLevel level);

/**
 * @brief Delete every log file.
 *
 * Blocks until the service has done it. `seq` and the chain are kept: the new
 * file starts with an `audit.cleared` event chained to the last deleted one.
 *
 * @return false if the SD card is not available or a file could not be removed
 */
bool audit_clear(Audit* audit);

#ifdef __cplusplus
}
#endif
