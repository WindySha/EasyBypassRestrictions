cmake_minimum_required(VERSION 3.10.2)

project("test-lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")

if (DEPENDENT_ON_LOCAL_LIBRARY)
    # import lib from local-project
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_DIRNAME)
    set(LIB_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../easyBypass)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(BYPASS_LIB_BASE ${LIB_BASE}/build/intermediates/merged_native_libs/debug)
    else()
        set(BYPASS_LIB_BASE ${LIB_BASE}/build/intermediates/stripped_native_libs/release)
    endif()

    add_library(easyBypass::easyBypass SHARED IMPORTED)
    set_target_properties(easyBypass::easyBypass PROPERTIES
            IMPORTED_LOCATION ${BYPASS_LIB_BASE}/out/lib/${ANDROID_ABI}/libeasyBypass.so
            INTERFACE_INCLUDE_DIRECTORIES ${LIB_BASE}/src/main/cpp/include)
else ()
    # import lib from maven
    find_package(easyBypass REQUIRED CONFIG)
endif ()

add_library(test-lib
        SHARED
        main.cpp)

find_library(log-lib
        log)

target_link_libraries(test-lib
        easyBypass::easyBypass
        ${log-lib})