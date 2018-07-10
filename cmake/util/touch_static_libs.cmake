# Create the static libs if they don't already exist to avoid not found errors
set(static_libs
        ${libidn_STATIC_LIBRARIES}
        ${jemalloc_STATIC_LIBRARIES}
        ${openssl_STATIC_LIBRARIES}
        ${curl_STATIC_LIBRARIES})

foreach(lib ${static_libs})
    if (NOT EXISTS ${lib})
        file(WRITE ${lib} "")
        message(STATUS "Creating ${lib}")
    endif()
endforeach()
