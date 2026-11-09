#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>

#include "../test.h" // IWYU pragma: keep

#include <applications/services/audit/audit_storage.h>

#define AUDIT_TEST_DIR EXT_PATH(".tmp/unit_tests/audit")

static const DateTime audit_test_datetime = {
    .hour = 1,
    .minute = 2,
    .second = 3,
    .day = 8,
    .month = 11,
    .year = 2026,
    .weekday = 7,
};

static bool audit_test_is_hex(const char* text, size_t len) {
    for(size_t i = 0; i < len; i++) {
        if(!((text[i] >= '0' && text[i] <= '9') || (text[i] >= 'a' && text[i] <= 'f'))) {
            return false;
        }
    }
    return text[len] == '\0';
}

static bool audit_test_file_exists(Storage* storage, uint8_t index) {
    FuriString* path = furi_string_alloc();
    audit_file_path(path, AUDIT_TEST_DIR, index);
    bool exists = storage_common_exists(storage, furi_string_get_cstr(path));
    furi_string_free(path);
    return exists;
}

MU_TEST(test_audit_sanitize) {
    char text[] = "a,b\"c\nd\re\x01 ok~";
    audit_sanitize(text);
    mu_assert_string_eq("a_b_c_d_e_ ok~", text);
}

MU_TEST(test_audit_format_and_parse) {
    FuriString* line = furi_string_alloc();
    AuditChain chain;
    audit_chain_reset(&chain);
    mu_assert_string_eq("0000000000000000", chain.chain);

    audit_format_line(
        line, &chain, &audit_test_datetime, 1234, AuditLevelWarning, "pin.fail", "count=1");

    mu_assert_int_eq(1, chain.seq);
    mu_assert(audit_test_is_hex(chain.chain, AUDIT_CHAIN_LEN), "chain is not 16 hex characters");
    mu_assert(
        furi_string_start_with_str(line, "1,2026-11-08T01:02:03,1234,warning,pin.fail,count=1,"),
        "unexpected line prefix");
    mu_assert(furi_string_end_with_str(line, "\n"), "line must end with a newline");

    AuditChain parsed;
    audit_chain_reset(&parsed);
    mu_assert(audit_parse_line(furi_string_get_cstr(line), &parsed), "line not parsed");
    mu_assert_int_eq(chain.seq, parsed.seq);
    mu_assert_string_eq(chain.chain, parsed.chain);

    furi_string_free(line);
}

MU_TEST(test_audit_chain) {
    FuriString* line = furi_string_alloc();
    AuditChain first, same, tampered;
    audit_chain_reset(&first);
    audit_chain_reset(&same);
    audit_chain_reset(&tampered);

    audit_format_line(line, &first, &audit_test_datetime, 10, AuditLevelInfo, "app.start", "x");
    audit_format_line(line, &same, &audit_test_datetime, 10, AuditLevelInfo, "app.start", "x");
    audit_format_line(line, &tampered, &audit_test_datetime, 10, AuditLevelInfo, "app.start", "y");

    // Deterministic for the same device and input, different when any field changes
    mu_assert_string_eq(first.chain, same.chain);
    mu_assert(strcmp(first.chain, tampered.chain) != 0, "tampered line kept the same chain");

    // The chain depends on the previous value
    AuditChain next = first;
    audit_format_line(line, &next, &audit_test_datetime, 10, AuditLevelInfo, "app.start", "x");
    mu_assert_int_eq(2, next.seq);
    mu_assert(strcmp(first.chain, next.chain) != 0, "chain did not advance");

    furi_string_free(line);
}

MU_TEST(test_audit_parse_rejects) {
    AuditChain chain;
    audit_chain_reset(&chain);
    mu_check(!audit_parse_line("# sentinel-audit v1", &chain));
    mu_check(!audit_parse_line("seq,time,uptime_ms,level,event,detail,chain", &chain));
    mu_check(!audit_parse_line("", &chain));
    mu_check(!audit_parse_line("5,2026-11-08T01:02:03,1,info,a,b,0123", &chain));
    mu_check(audit_parse_line("7,2026-11-08T01:02:03,1,info,a,b,0123456789abcdef\r\n", &chain));
    mu_assert_int_eq(7, chain.seq);
    mu_assert_string_eq("0123456789abcdef", chain.chain);
}

MU_TEST(test_audit_verify) {
    FuriString* lines[3];
    AuditChain writer;
    audit_chain_reset(&writer);
    for(size_t i = 0; i < COUNT_OF(lines); i++) {
        lines[i] = furi_string_alloc();
        audit_format_line(
            lines[i], &writer, &audit_test_datetime, i, AuditLevelInfo, "test.event", "detail");
    }

    // An untouched log verifies line by line
    AuditChain chain;
    audit_chain_reset(&chain);
    for(size_t i = 0; i < COUNT_OF(lines); i++) {
        mu_assert_int_eq(AuditVerifyOk, audit_verify_line(furi_string_get_cstr(lines[i]), &chain));
    }
    mu_assert_int_eq(3, chain.seq);

    // A modified field is detected, and the next line still verifies
    FuriString* tampered = furi_string_alloc_set(lines[1]);
    furi_string_replace_str(tampered, "detail", "detaiX", 0);
    audit_chain_reset(&chain);
    mu_assert_int_eq(AuditVerifyOk, audit_verify_line(furi_string_get_cstr(lines[0]), &chain));
    mu_assert_int_eq(
        AuditVerifyMacMismatch, audit_verify_line(furi_string_get_cstr(tampered), &chain));
    mu_assert_int_eq(AuditVerifyOk, audit_verify_line(furi_string_get_cstr(lines[2]), &chain));

    // A deleted line is reported as a gap
    audit_chain_reset(&chain);
    mu_assert_int_eq(AuditVerifyOk, audit_verify_line(furi_string_get_cstr(lines[0]), &chain));
    mu_assert_int_eq(AuditVerifySeqGap, audit_verify_line(furi_string_get_cstr(lines[2]), &chain));

    // Garbage and oversized lines are rejected without touching the chain
    AuditChain before = chain;
    mu_assert_int_eq(AuditVerifyMalformed, audit_verify_line("garbage", &chain));
    FuriString* oversized = furi_string_alloc_set("9,");
    for(size_t i = 0; i < 30; i++) {
        furi_string_cat_str(oversized, "0123456789");
    }
    furi_string_cat_str(oversized, ",0123456789abcdef");
    mu_assert_int_eq(
        AuditVerifyMalformed, audit_verify_line(furi_string_get_cstr(oversized), &chain));
    mu_assert_int_eq(before.seq, chain.seq);

    furi_string_free(oversized);
    furi_string_free(tampered);
    for(size_t i = 0; i < COUNT_OF(lines); i++) {
        furi_string_free(lines[i]);
    }
}

MU_TEST(test_audit_rotation_and_restore) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_remove_recursive(storage, AUDIT_TEST_DIR);

    AuditStorage* writer = audit_storage_alloc(storage, AUDIT_TEST_DIR, 1024, 3);
    mu_assert(audit_storage_open(writer), "open failed");
    mu_assert_int_eq(0, audit_storage_get_chain(writer)->seq);

    mu_assert(audit_storage_begin(writer), "begin failed");
    for(uint32_t i = 0; i < 40; i++) {
        mu_assert(
            audit_storage_write(
                writer, &audit_test_datetime, i, AuditLevelInfo, "test.event", "rotation"),
            "write failed");
    }
    audit_storage_end(writer);
    AuditChain written = *audit_storage_get_chain(writer);
    audit_storage_free(writer);

    // 40 events plus one audit.rotate per rotation
    mu_assert(written.seq > 40, "no rotation happened");
    mu_check(audit_test_file_exists(storage, 0));
    mu_check(audit_test_file_exists(storage, 1));
    mu_check(audit_test_file_exists(storage, 2));
    mu_check(!audit_test_file_exists(storage, 3));

    // A new writer continues the same chain
    AuditStorage* reader = audit_storage_alloc(storage, AUDIT_TEST_DIR, 1024, 3);
    mu_assert(audit_storage_open(reader), "reopen failed");
    mu_assert_int_eq(written.seq, audit_storage_get_chain(reader)->seq);
    mu_assert_string_eq(written.chain, audit_storage_get_chain(reader)->chain);
    audit_storage_free(reader);

    storage_simply_remove_recursive(storage, AUDIT_TEST_DIR);
    furi_record_close(RECORD_STORAGE);
}

MU_TEST(test_audit_unterminated_line) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_remove_recursive(storage, AUDIT_TEST_DIR);
    mu_check(storage_simply_mkdir(storage, EXT_PATH(".tmp/unit_tests")));
    mu_check(storage_simply_mkdir(storage, AUDIT_TEST_DIR));

    // Simulate a write cut by power loss
    FuriString* path = furi_string_alloc();
    audit_file_path(path, AUDIT_TEST_DIR, 0);
    File* file = storage_file_alloc(storage);
    const char* content = AUDIT_HEADER "9,2026-11-08T01:02:03,1,info,a,b,0123456789abcdef\n"
                                       "10,2026-11-08T01:02";
    mu_check(storage_file_open(file, furi_string_get_cstr(path), FSAM_WRITE, FSOM_CREATE_ALWAYS));
    mu_check(storage_file_write(file, content, strlen(content)) == strlen(content));
    storage_file_close(file);

    AuditStorage* writer = audit_storage_alloc(storage, AUDIT_TEST_DIR, 4096, 2);
    mu_assert(audit_storage_open(writer), "open failed");
    mu_assert_int_eq(9, audit_storage_get_chain(writer)->seq);
    mu_assert(audit_storage_begin(writer), "begin failed");
    mu_check(audit_storage_write(writer, &audit_test_datetime, 1, AuditLevelInfo, "a", "b"));
    audit_storage_end(writer);
    mu_assert_int_eq(10, audit_storage_get_chain(writer)->seq);
    audit_storage_free(writer);

    // The partial line was terminated, so the new event starts on its own line
    mu_check(storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING));
    char buffer[256] = {0};
    storage_file_read(file, buffer, sizeof(buffer) - 1);
    storage_file_close(file);
    mu_assert(strstr(buffer, "10,2026-11-08T01:02\n10,2026-11-08T01:02:03,") != NULL, buffer);

    storage_file_free(file);
    furi_string_free(path);
    storage_simply_remove_recursive(storage, AUDIT_TEST_DIR);
    furi_record_close(RECORD_STORAGE);
}

MU_TEST_SUITE(test_audit_suite) {
    MU_RUN_TEST(test_audit_sanitize);
    MU_RUN_TEST(test_audit_format_and_parse);
    MU_RUN_TEST(test_audit_chain);
    MU_RUN_TEST(test_audit_parse_rejects);
    MU_RUN_TEST(test_audit_verify);
    MU_RUN_TEST(test_audit_rotation_and_restore);
    MU_RUN_TEST(test_audit_unterminated_line);
}

int run_minunit_test_audit(void) {
    MU_RUN_SUITE(test_audit_suite);
    return MU_EXIT_CODE;
}

TEST_API_DEFINE(run_minunit_test_audit)
