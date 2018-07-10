include(ExternalProject)

set(curl_REPOSITORY "https://github.com/curl/curl.git")
set(curl_TAG "curl-7_54_1")
set(curl_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/curl)
set(curl_SOURCE_DIR ${curl_ROOT_DIR}/src)
set(curl_INSTALL_DIR ${curl_ROOT_DIR}/install)
set(curl_STATIC_LIBRARIES ${curl_INSTALL_DIR}/lib/libcurl.a)
set(curl_INCLUDE_DIR ${curl_INSTALL_DIR}/include)
set(CONFIGURE_COMMAND_ARGS
        --prefix=${curl_INSTALL_DIR}
        --without-gnutls
        --with-ssl
        --without-nghttp2
        --without-librtmp
        --disable-ldap
        --disable-shared
        "CC=ccache gcc"
        "CXX=ccache g++")


ExternalProject_Add(curl
        PREFIX curl
        GIT_REPOSITORY ${curl_REPOSITORY}
        GIT_TAG ${curl_TAG}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        PATCH_COMMAND ${curl_SOURCE_DIR}/curl/buildconf
        CONFIGURE_COMMAND ${curl_SOURCE_DIR}/curl/configure ${CONFIGURE_COMMAND_ARGS}
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        CMAKE_CACHE_ARGS
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_BUILD_TYPE:STRING=Release)


function(get_curl_link_flags curl_LINK_FLAGS)
    set(curl_flags)
    set(dl_libs)
    execute_process(COMMAND ${curl_INSTALL_DIR}/bin/curl-config --static-libs
            OUTPUT_VARIABLE flags
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    separate_arguments(flags)
    foreach(flag ${flags})
        if(${flag} STREQUAL -lz)
            list(APPEND curl_flags ${gotten_zlib_LINK_FLAGS})

        elseif(${flag} STREQUAL -lidn)
            list(APPEND curl_flags ${gotten_libidn_LINK_FLAGS})

        elseif(${flag} STREQUAL -ldl)
            list(APPEND dl_libs -ldl) # Linking may fail if -ldl isn't last

        else()
            list(APPEND curl_flags ${flag})

        endif()
    endforeach()
    string(STRIP "${flags} ${dl_libs}" stripped_flags)
    set(${curl_LINK_FLAGS} ${stripped_flags} PARENT_SCOPE)
endfunction()

get_curl_link_flags(gotten_curl_LINK_FLAGS)

