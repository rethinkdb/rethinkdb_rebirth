include(ExternalProject)

set(boost_VERSION "1.60.0")
string(REGEX REPLACE "\\." "_" boost_VERSION_UNDERSCORED ${boost_VERSION})
set(boost_URL "https://tenet.dl.sourceforge.net/project/boost/boost/${boost_VERSION}/boost_${boost_VERSION_UNDERSCORED}.tar.bz2")
set(boost_SHA1 "7f56ab507d3258610391b47fef6b11635861175a")
set(boost_CONFIGURE_COMMAND ./bootstrap.sh)
set(boost_BUILD_COMMAND ./b2)
set(boost_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/boost/install)
set(boost_INCLUDE_DIR ${boost_INSTALL_DIR}/include)
set(boost_STATIC_LIB_DIR ${boost_INSTALL_DIR}/lib)
set(boost_STATIC_LIBRARIES ${boost_STATIC_LIB_DIR}/libboost_system.a)

ExternalProject_Add(boost
        PREFIX boost
        BUILD_IN_SOURCE 1
        URL ${boost_URL}
        URL_HASH SHA1=${boost_SHA1}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        CONFIGURE_COMMAND ${boost_CONFIGURE_COMMAND} --with-libraries=system --prefix=${boost_STATIC_LIB_DIR}
        BUILD_COMMAND ${boost_BUILD_COMMAND} install -j8 --prefix=${boost_INSTALL_DIR}
        INSTALL_COMMAND ""
        CMAKE_CACHE_ARGS
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_BUILD_TYPE:STRING=Release)

