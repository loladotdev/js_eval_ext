cmake_minimum_required(VERSION 3.16)

project(js_eval_ext)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# SQLite3
add_library(sqlite3 SHARED sqlite3/sqlite3.c)
target_include_directories(sqlite3 PUBLIC sqlite3)
target_compile_definitions(sqlite3 PUBLIC SQLITE_ENABLE_COLUMN_METADATA=1)

# Duktape
add_library(duktape STATIC duktape/duktape.c)
target_include_directories(duktape PUBLIC duktape)

# eval_js
add_library(js_eval_ext SHARED js_eval_ext.c)
target_link_libraries(js_eval_ext PUBLIC sqlite3 duktape m)

# Unit tests
add_executable(test_js_eval_ext test_js_eval_ext.c)
target_link_libraries(test_js_eval_ext PRIVATE sqlite3 duktape js_eval_ext)