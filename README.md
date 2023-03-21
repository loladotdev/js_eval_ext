# SQLite3 JavaScript Eval Extension

This SQLite3 extension allows you to evaluate JavaScript expressions using the Duktape library.

## Prerequisites

- CMake >= 3.16

## Building

To build the extension, follow these steps:

1. Create a build directory:
   ```shell
   mkdir build
   cd build
   ```
2. Generate the build files using CMake:
    ```shell
    cmake ..
    ```
3. Build the extension:
    ```shell
    make
    ```

This will create the `libeval_js.so` shared library in the `build` directory.

## Usage

1. Load the extension in SQLite3:
   ```sql
   SELECT load_extension('/path/to/libjs_eval_ext.so', 'sqlite3_js_eval_init');
   ```
   Replace /path/to/libeval_js.so with the actual path to the built libeval_js.so file.

2. Use the eval_js function to evaluate JavaScript expressions:
    ```sql
   SELECT eval_js('a * (b + c)', 'a', 2, 'b', 3, 'c', 4) AS result;
    ```
   This will evaluate the JavaScript expression a * (b + c) with the provided key-value pairs as variables (a = 2, b =
   3, c = 4) and return the result (14).

## Running Tests

To run the tests, execute the following command in the build directory:

```shell
ctest
```

This will run the test suite and display the results.
