set(DOWNLOAD_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/downloads"
        CACHE PATH "Location where external projects will be downloaded.")

# Version
set(gen_version ${PROJECT_SOURCE_DIR}/scripts/gen-version.sh)
execute_process(COMMAND bash ${gen_version}
        OUTPUT_VARIABLE RETHINKDB_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)

set(PRODUCT_NAME "RethinkDB")

set(GDB_FUNCTIONS_NAME "rethinkdb-gdb.py")
