cmake_minimum_required(VERSION 3.15)

project(ALU74181 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(74181
    ../../74181.c
)

if (MSVC)
    target_compile_options(74181 PRIVATE /W4 /WX)
else()
    target_compile_options(74181 PRIVATE -Wall -Wextra -Werror)
endif()
