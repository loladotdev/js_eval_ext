#include "duktape.h"
#include "sqlite3ext.h"
#include <stddef.h>

SQLITE_EXTENSION_INIT1

static void js_eval(
        sqlite3_context *ctx,
        int argc,
        sqlite3_value **argv) {

    if (argc % 2 != 1) {
        sqlite3_result_error(ctx, "Incorrect number of arguments. Expected an odd number of arguments: the JS expression, followed by key/value pairs.", -1);
        return;
    }

    duk_context *duk_ctx = duk_create_heap_default();

    if (!duk_ctx) {
        sqlite3_result_error(ctx, "Failed to create Duktape heap", -1);
        return;
    }

    // Get the JavaScript expression
    const char *js_expr = (const char *) sqlite3_value_text(argv[0]);
    duk_push_lstring(duk_ctx, js_expr, sqlite3_value_bytes(argv[0]));

    // Process the key-value pairs
    for (int i = 1; i < argc; i += 2) {
        const char *key = (const char *) sqlite3_value_text(argv[i]);

        if (!key) {
            duk_destroy_heap(duk_ctx);
            sqlite3_result_error(ctx, "Invalid key in key/value pair.", -1);
            return;
        }

        int value_type = sqlite3_value_type(argv[i + 1]);

        switch (value_type) {
            case SQLITE_INTEGER:
                duk_push_int(duk_ctx, sqlite3_value_int(argv[i + 1]));
                break;
            case SQLITE_FLOAT:
                duk_push_number(duk_ctx, sqlite3_value_double(argv[i + 1]));
                break;
            case SQLITE_TEXT:
                duk_push_lstring(duk_ctx, (const char *) sqlite3_value_text(argv[i + 1]), sqlite3_value_bytes(argv[i + 1]));
                break;
            default:
                duk_push_undefined(duk_ctx);
                break;
        }

        duk_put_global_string(duk_ctx, key);
    }

    if (duk_peval(duk_ctx) != 0) {
        sqlite3_result_error(ctx, duk_safe_to_string(duk_ctx, -1), -1);
    } else {
        // Handle the return type correctly
        if (duk_is_boolean(duk_ctx, -1)) {
            sqlite3_result_int(ctx, duk_to_boolean(duk_ctx, -1));
        } else if (duk_is_number(duk_ctx, -1)) {
            double num = duk_get_number(duk_ctx, -1);
            if (floor(num) == num) {
                sqlite3_result_int64(ctx, (sqlite3_int64) num);
            } else {
                sqlite3_result_double(ctx, num);
            }
        } else {
            // Use TEXT as the fallback for string, null, and other types
            sqlite3_result_text(ctx, duk_to_string(duk_ctx, -1), -1, SQLITE_TRANSIENT);
        }
    }

    duk_destroy_heap(duk_ctx);
}


SQLITE_API int sqlite3_js_eval_init(
        sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) {
    SQLITE_EXTENSION_INIT2(pApi)
    return sqlite3_create_function_v2(
            db, "eval_js", -1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0, js_eval, 0, 0, 0);
}
