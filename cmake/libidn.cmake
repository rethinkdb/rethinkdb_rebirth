include(ExternalProject)

set(libidn_VERSION "1.28")
set(libidn_URL "http://ftp.gnu.org/gnu/libidn/libidn-${libidn_VERSION}.tar.gz")
set(libidn_SHA1 "725587211b229c156e29fa2ad116b0ef71a7ca17")
set(libidn_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/libidn)
set(libidn_INSTALL_DIR ${libidn_ROOT_DIR}/install)
set(libidn_INCLUDE_DIR ${libidn_INSTALL_DIR}/include)
set(libidn_LIB_DIR ${libidn_INSTALL_DIR}/lib)
set(libidn_STATIC_LIBRARIES ${libidn_LIB_DIR}/libidn.a)
set(CONFIGURE_COMMAND_ARGS
        --prefix=${libidn_INSTALL_DIR}
        "CC=ccache gcc"
        "CXX=ccache g++")

ExternalProject_Add(libidn
        PREFIX libidn
        URL ${libidn_URL}
        URL_HASH SHA1=${libidn_SHA1}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./configure ${CONFIGURE_COMMAND_ARGS}
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        CMAKE_CACHE_ARGS
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_BUILD_TYPE:STRING=Release)


function(get_libidn_link_flags libidn_LINK_FLAGS)
    unset(WITHOUT_ICONV)
    unset(WITH_ICONV)
    set(libidn_TEMP_DIR ${libidn_ROOT_DIR}/tmp)
    set(libidn_IN_FILE ${libidn_TEMP_DIR}/libiconv_check.c)
    add_custom_target(make-temp-directory ALL
            COMMAND ${CMAKE_COMMAND} -E make_directory ${libidn_TEMP_DIR})
    file(WRITE ${libidn_IN_FILE}
            "#include <iconv.h>\n int main(void) { iconv_t sample; sample = iconv_open(\"UTF-8\", \"ASCII\"); return 0; }")
    try_compile(WITHOUT_ICONV ${CMAKE_CURRENT_BINARY_DIR} ${libidn_IN_FILE}
            CMAKE_FLAGS
            -DLINK_LIBRARIES=-pthread)
    if (WITHOUT_ICONV)
        set(${libidn_LINK_FLAGS} ${libidn_STATIC_LIBRARIES} PARENT_SCOPE)
    else()
        try_compile(WITH_ICONV ${libidn_IN_FILE}
                LINK_LIBRARIES iconv)
        if(WITH_ICONV)
            set(${libidn_LINK_FLAGS} ${libidn_STATIC_LIBRARIES} PARENT_SCOPE)
        else()
            message(SEND_ERROR "Error: Unable to figure out how to link libiconv")
            return()
        endif()
    endif()
    unset(WITHOUT_ICONV)
    unset(WITH_ICONV)
    add_custom_target(remove-temp-directory ALL
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${libidn_TEMP_DIR})
endfunction()

get_libidn_link_flags(gotten_libidn_LINK_FLAGS)