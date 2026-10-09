cmake_minimum_required(VERSION 3.15)

project(ALU74181 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(74181
    74181.c
)

if (MSVC)
    target_compile_options(74181 PRIVATE /W4 /WX)
    target_compile_definitions(74181 PRIVATE _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN PLATFORM_WINDOWS)
else()
    target_compile_options(74181 PRIVATE -Wall -Wextra -Werror)
    target_compile_definitions(74181 PRIVATE PLATFORM_UNIX)
endif()

if (MSVC)
    message(STATUS "Compilazione su Windows (MSVC)")
elseif (WIN32)
    message(STATUS "Compilazione su Windows (${CMAKE_C_COMPILER_ID})")
else()
    message(STATUS "Compilazione su sistema Unix/Linux")
endif()

enable_testing()

add_executable(test_alu74181 tests/test_alu74181.c)

if (MSVC)
    target_compile_options(test_alu74181 PRIVATE /W4 /WX)
    target_compile_definitions(test_alu74181 PRIVATE _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN PLATFORM_WINDOWS)
else()
    target_compile_options(test_alu74181 PRIVATE -Wall -Wextra -Werror)
    target_compile_definitions(test_alu74181 PRIVATE PLATFORM_UNIX)
endif()

add_test(NAME test_alu74181 COMMAND test_alu74181)
