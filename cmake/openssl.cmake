include(ExternalProject)

set(openssl_REPOSITORY "https://github.com/openssl/openssl.git")
set(openssl_TAG "OpenSSL_1_0_2l")
set(openssl_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
set(openssl_INSTALL_DIR ${openssl_ROOT_DIR}/install)
set(openssl_INCLUDE_DIRS ${openssl_INSTALL_DIR}/include)
set(openssl_LIB_DIR ${openssl_INSTALL_DIR}/lib)
set(openssl_STATIC_LIBRARIES
        ${openssl_LIB_DIR}/libcrypto.a
        ${openssl_LIB_DIR}/libssl.a)


ExternalProject_Add(openssl
        PREFIX openssl
        GIT_REPOSITORY ${openssl_REPOSITORY}
        GIT_TAG ${openssl_TAG}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./config shared --prefix=${openssl_INSTALL_DIR}
        BUILD_COMMAND make -j1
        INSTALL_COMMAND make install
        CMAKE_CACHE_ARGS
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_BUILD_TYPE:STRING=Release)


function(get_openssl_link_flags openssl_LINK_FLAGS)
    set(dl_lib)
    if (${UNIX})
        set(dl_lib -ldl)
    endif()
    set(${openssl_LINK_FLAGS} ${dl_lib} PARENT_SCOPE)
endfunction()

get_openssl_link_flags(gotten_openssl_LINK_FLAGS)