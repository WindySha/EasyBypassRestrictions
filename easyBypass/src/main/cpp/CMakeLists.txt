cmake_minimum_required(VERSION 3.10.2)
project("easyBypass")

enable_language(ASM)

add_library(easyBypass
        SHARED
        main.c
        bypassed_dlfcn.c
        fake_dlfcn.c
        )

find_library(log-lib
              log )

#target_compile_options(easybypass PUBLIC -Weverything -Werror)
target_include_directories(easyBypass PUBLIC include .)
target_link_libraries(easyBypass ${log-lib} )

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O3 -flto -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -flto -fvisibility=hidden -fvisibility-inlines-hidden")
