cmake_minimum_required(VERSION 3.16.3)
project(brotli VERSION 1.2.0 LANGUAGES C)

message(STATUS "Configuring brotli")

set(CMAKE_BUILD_TYPE "Release")

file(GLOB_RECURSE BROTLI_COMMON_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} c/common/*.c)
file(GLOB_RECURSE BROTLI_DEC_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} c/dec/*.c)
file(GLOB_RECURSE BROTLI_ENC_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} c/enc/*.c)

if(MSVC)
    message(STATUS "Defining _CRT_SECURE_NO_WARNINGS to avoid warnings about security")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(BROTLI_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/c/include")

if(BROTLI_PARENT_DIRECTORY)
    set(BROTLI_INCLUDE_DIRS "${BROTLI_INCLUDE_DIRS}" PARENT_SCOPE)
    set(BROTLI_LIBRARIES "${BROTLI_LIBRARIES}" PARENT_SCOPE)
endif()

add_library(brotlicommon-object OBJECT ${BROTLI_COMMON_SOURCES})
add_library(brotlidec-object OBJECT ${BROTLI_DEC_SOURCES})
add_library(brotlienc-object OBJECT ${BROTLI_ENC_SOURCES})

target_include_directories(brotlicommon-object PRIVATE ${BROTLI_INCLUDE_DIRS})
target_include_directories(brotlidec-object PRIVATE ${BROTLI_INCLUDE_DIRS})
target_include_directories(brotlienc-object PRIVATE ${BROTLI_INCLUDE_DIRS})

if(ENABLE_SANITIZER)
    target_compile_options(brotlicommon-object PUBLIC -fsanitize=${ENABLE_SANITIZER})
    target_compile_options(brotlidec-object PUBLIC -fsanitize=${ENABLE_SANITIZER})
    target_compile_options(brotlienc-object PUBLIC -fsanitize=${ENABLE_SANITIZER})
    if(WIN32)
        target_link_options(brotlicommon-object PUBLIC -fsanitize=${ENABLE_SANITIZER})
        target_link_options(brotlidec-object PUBLIC -fsanitize=${ENABLE_SANITIZER})
        target_link_options(brotlienc-object PUBLIC -fsanitize=${ENABLE_SANITIZER})
    endif()
endif()
