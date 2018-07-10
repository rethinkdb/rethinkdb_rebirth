include (ExternalProject)

set(gtest_REPOSITORY "https://github.com/google/googletest.git")
set(gtest_TAG "release-1.7.0")
set(gtest_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest/include)
set(gtest_BUILD ${CMAKE_CURRENT_BINARY_DIR}/gtest)
set(gtest_STATIC_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest/libgtest.a)


ExternalProject_Add(gtest
        PREFIX gtest
        GIT_REPOSITORY ${gtest_REPOSITORY}
        GIT_TAG ${gtest_TAG}
        DOWNLOAD_DIR "${DOWNLOAD_LOCATION}"
        BUILD_IN_SOURCE 1
        BUILD_BY_PRODUCTS ${gtest_STATIC_LIBRARIES}
        INSTALL_COMMAND ""
        CMAKE_CACHE_ARGS
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DBUILD_GMOCK:BOOL=OFF
            -DBUILD_GTEST:BOOL=ON
            -Dgtest_force_shared_crt:BOOL=ON)