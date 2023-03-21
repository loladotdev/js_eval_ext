#include "sqlite3.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void test_eval_js_integer(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, "SELECT eval_js('a * (b + c)', 'a', 2, 'b', 3, 'c', 4) AS result;", -1, &stmt, NULL);
    assert(rc == SQLITE_OK);

    rc = sqlite3_step(stmt);
    assert(rc == SQLITE_ROW);
    assert(sqlite3_column_int(stmt, 0) == 14);

    sqlite3_finalize(stmt);
}

void test_eval_js_real(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, "SELECT eval_js('a / b', 'a', 7.0, 'b', 3.0) AS result;", -1, &stmt, NULL);
    assert(rc == SQLITE_OK);

    rc = sqlite3_step(stmt);
    assert(rc == SQLITE_ROW);
    double real_result = sqlite3_column_double(stmt, 0);
    assert(fabs(real_result - (7.0 / 3.0)) < 1e-6);

    sqlite3_finalize(stmt);
}

void test_eval_js_string(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, "SELECT eval_js('a + b', 'a', 'Hello, ', 'b', 'world!') AS result;", -1, &stmt, NULL);
    assert(rc == SQLITE_OK);

    rc = sqlite3_step(stmt);
    assert(rc == SQLITE_ROW);
    assert(strcmp((const char *) sqlite3_column_text(stmt, 0), "Hello, world!") == 0);

    sqlite3_finalize(stmt);
}

void test_eval_js_fallback(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, "SELECT eval_js('undefined') AS result;", -1, &stmt, NULL);
    assert(rc == SQLITE_OK);

    rc = sqlite3_step(stmt);
    assert(rc == SQLITE_ROW);
    assert(strcmp((const char *) sqlite3_column_text(stmt, 0), "undefined") == 0);

    sqlite3_finalize(stmt);
}

void test_eval_js_from_table_strings(sqlite3 *db) {
    sqlite3_stmt *stmt;
    int rc;

    // Create a table and insert some data
    rc = sqlite3_exec(db, "CREATE TABLE test_data_strings (str1 TEXT, str2 TEXT);", NULL, NULL, NULL);
    assert(rc == SQLITE_OK);

    rc = sqlite3_exec(db, "INSERT INTO test_data_strings (str1, str2) VALUES ('hello,world', 'john,doe');", NULL, NULL, NULL);
    assert(rc == SQLITE_OK);

    // Use eval_js function with values from the table and perform string manipulation
    rc = sqlite3_prepare_v2(db, "SELECT eval_js('str1.split(\",\")[0] + \" \" + str2.split(\",\")[1]', 'str1', str1, 'str2', str2) AS result FROM test_data_strings;", -1, &stmt, NULL);
    assert(rc == SQLITE_OK);

    rc = sqlite3_step(stmt);
    assert(rc == SQLITE_ROW);
    assert(strcmp((const char *) sqlite3_column_text(stmt, 0), "hello doe") == 0);

    sqlite3_finalize(stmt);
}

int main() {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open(":memory:", &db);
    assert(rc == SQLITE_OK);

    rc = sqlite3_enable_load_extension(db, 1);
    assert(rc == SQLITE_OK);

    rc = sqlite3_load_extension(db, "./libjs_eval_ext.so", "sqlite3_js_eval_init", 0);
    assert(rc == SQLITE_OK);

    test_eval_js_integer(db);
    test_eval_js_real(db);
    test_eval_js_string(db);
    test_eval_js_fallback(db);
    test_eval_js_from_table_strings(db);

    sqlite3_close(db);

    printf("All tests passed.\n");
    return 0;
}
