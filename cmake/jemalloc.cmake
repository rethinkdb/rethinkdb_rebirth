include (ExternalProject)

set(jemalloc_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/jemalloc/src/jemalloc/include)
set(jemalloc_URL https://github.com/jemalloc/jemalloc.git)
set(jemalloc_TAG 4.5.0)
set(jemalloc_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/jemalloc)
set(jemalloc_BUILD ${jemalloc_ROOT_DIR}/src/jemalloc)
set(jemalloc_INSTALL_DIR ${jemalloc_ROOT_DIR}/install)
set(jemalloc_STATIC_LIB_DIR ${jemalloc_INSTALL_DIR}/lib)
set(jemalloc_STATIC_LIBRARIES ${jemalloc_STATIC_LIB_DIR}/libjemalloc.a)

set(CONFIGURE_COMMAND_ARGS
        --prefix=${jemalloc_INSTALL_DIR}
        --libdir=${jemalloc_STATIC_LIB_DIR}
        )

ExternalProject_Add(jemalloc
        PREFIX jemalloc
        GIT_REPOSITORY ${jemalloc_URL}
        GIT_TAG ${jemalloc_TAG}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./autogen.sh ${CONFIGURE_COMMAND_ARGS}
        BUILD_COMMAND make
        INSTALL_COMMAND make install_bin install_include install_lib
        CMAKE_CACHE_ARGS
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_BUILD_TYPE:STRING=Release
        )

function(get_jemalloc_LINK_FLAGS jemalloc_LINK_FLAGS)
    set(link_flags "-Wl,--no-as-needed")

    set(${jemalloc_LINK_FLAGS} ${link_flags} PARENT_SCOPE)
endfunction()

get_jemalloc_LINK_FLAGS(gotten_jemalloc_LINK_FLAGS)



