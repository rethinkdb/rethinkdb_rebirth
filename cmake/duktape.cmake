include (ExternalProject)

set(duktape_URL "https://github.com/RebirthDB/rebirthdb-vendored.git")
set(duktape_TAG "duktape-2.2.1")
set(duktape_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/duktape/src/duktape/duktape/src)
set(duktape_SRC ${duktape_INCLUDE_DIR}/duktape.c)
set_source_files_properties(${duktape_SRC} PROPERTIES GENERATED TRUE)

ExternalProject_Add(duktape
        PREFIX duktape
        GIT_REPOSITORY ${duktape_URL}
        GIT_TAG ${duktape_TAG}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND "")



