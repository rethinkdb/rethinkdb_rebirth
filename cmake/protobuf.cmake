include (ExternalProject)

set(protobuf_VERSION "2.5.0")
set(protobuf_URL "https://github.com/google/protobuf/releases/download/v${protobuf_VERSION}/protobuf-${protobuf_VERSION}.tar.bz2")
set(protobuf_SHA1 "62c10dcdac4b69cc8c6bb19f73db40c264cb2726")
set(protobuf_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf)
set(protobuf_INSTALL_DIR ${protobuf_ROOT_DIR}/install)
set(protobuf_LIB_DIR ${protobuf_INSTALL_DIR}/lib)
set(protobuf_INCLUDE_DIRS ${protobuf_INSTALL_DIR}/include)
set(protobuf_STATIC_LIBRARIES ${protobuf_INSTALL_DIR}/lib/libprotobuf.a)
set(protobuf_PROTOC_EXECUTABLE ${protobuf_INSTALL_DIR}/bin/protoc)
set(CONFIGURE_COMMAND_ARGS
        --prefix ${protobuf_INSTALL_DIR}
        --libdir ${protobuf_LIB_DIR}
        --enable-static
        --disable-shared
        "CC=ccache gcc"
        "CXX=ccache g++"
        )

ExternalProject_Add(protobuf
        PREFIX protobuf
        URL ${protobuf_URL}
        URL_HASH SHA1=${protobuf_SHA1}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./configure ${CONFIGURE_COMMAND_ARGS}
        BUILD_COMMAND make
        INSTALL_COMMAND make install
        CMAKE_CACHE_ARGS
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_BUILD_TYPE:STRING=Release
        )



# A variant of PROTOBUF_GENERATE_CPP that keeps the directory hierarchy.
# ROOT_DIR must be absolute, and proto paths must be relative to ROOT_DIR
function(RELATIVE_PROTOBUF_GENERATE_CPP SRCS HDRS ROOT_DIR)
    if(NOT ARGN)
        message(SEND_ERROR "Error: RELATIVE_PROTOBUF_GENERATE_CPP() called without any proto files")
        return()
    endif()

    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        set(ABS_FIL ${ROOT_DIR}/${FIL})
        get_filename_component(FIL_WE ${FIL} NAME_WE)
        get_filename_component(FIL_DIR ${ABS_FIL} PATH)
        file(RELATIVE_PATH REL_DIR ${ROOT_DIR} ${FIL_DIR})

        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${REL_DIR}/${FIL_WE}.pb.cc")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${REL_DIR}/${FIL_WE}.pb.h")

        add_custom_command(
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${REL_DIR}/${FIL_WE}.pb.cc"
                "${CMAKE_CURRENT_BINARY_DIR}/${REL_DIR}/${FIL_WE}.pb.h"
                COMMAND  ${protobuf_PROTOC_EXECUTABLE}
                ARGS --cpp_out  ${CMAKE_CURRENT_BINARY_DIR} -I ${ROOT_DIR} ${ABS_FIL} -I ${protobuf_INCLUDE_DIRS}
                DEPENDS ${ABS_FIL} protobuf
                COMMENT "Running C++ protocol buffer compiler on ${FIL}"
                VERBATIM )
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
    set(proto_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endfunction()

set(ql2_proto_src rdb_protocol/ql2.proto)
set(proto_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/rdb_protocol)

RELATIVE_PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HDR
        ${PROJECT_SOURCE_DIR}/src ${ql2_proto_src})