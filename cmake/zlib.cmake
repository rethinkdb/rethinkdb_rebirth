include (ExternalProject)

set(zlib_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/zlib_archive)
set(ZLIB_URL https://github.com/madler/zlib)
set(ZLIB_BUILD ${CMAKE_CURRENT_BINARY_DIR}/zlib/src/zlib)
set(ZLIB_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/zlib/install)
set(ZLIB_TAG v1.2.11)

set(zlib_STATIC_LIBRARIES
        ${CMAKE_CURRENT_BINARY_DIR}/zlib/install/lib/libz.a)

set(ZLIB_HEADERS
        "${ZLIB_INSTALL}/include/zconf.h"
        "${ZLIB_INSTALL}/include/zlib.h")

ExternalProject_Add(zlib
        PREFIX zlib
        GIT_REPOSITORY ${ZLIB_URL}
        GIT_TAG ${ZLIB_TAG}
        INSTALL_DIR ${ZLIB_INSTALL}
        BUILD_IN_SOURCE 1
        BUILD_BYPRODUCTS ${zlib_STATIC_LIBRARIES}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        CMAKE_CACHE_ARGS
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
        -DCMAKE_BUILD_TYPE:STRING=Release
        -DCMAKE_INSTALL_PREFIX:STRING=${ZLIB_INSTALL}
        )

add_custom_target(zlib_create_destination_dir
        COMMAND ${CMAKE_COMMAND} -E make_directory ${zlib_INCLUDE_DIR}
        DEPENDS zlib)

add_custom_target(zlib_copy_headers_to_destination
        DEPENDS zlib_create_destination_dir)

foreach(header_file ${ZLIB_HEADERS})
    add_custom_command(TARGET zlib_copy_headers_to_destination PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${header_file} ${zlib_INCLUDE_DIR})
endforeach()


function(get_zlib_link_flags zlib_LINK_FLAGS)
    set(${zlib_LINK_FLAGS} ${zlib_STATIC_LIBRARIES} PARENT_SCOPE)
endfunction()

get_zlib_link_flags(gotten_zlib_LINK_FLAGS)
