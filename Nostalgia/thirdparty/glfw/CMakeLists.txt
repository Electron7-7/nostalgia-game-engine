cmake_minimum_required(VERSION 3.16...3.28 FATAL_ERROR)
project(GLFW VERSION 3.5.0 LANGUAGES C HOMEPAGE_URL "https://www.glfw.org/")
message(STATUS "Configuring GLFW")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

string(COMPARE EQUAL "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" GLFW_STANDALONE)

include(GNUInstallDirs)
include(CMakeDependentOption)

if(GLFW_USE_OSMESA)
    message(FATAL_ERROR "GLFW_USE_OSMESA has been removed; set the GLFW_PLATFORM init hint")
endif()

if(DEFINED GLFW_USE_WAYLAND AND UNIX AND NOT APPLE)
    message(FATAL_ERROR
        "GLFW_USE_WAYLAND has been removed; delete the CMake cache and set GLFW_BUILD_WAYLAND and GLFW_BUILD_X11 instead")
endif()

if(WIN32)
    set(GLFW_BUILD_WIN32 ON)
elseif(UNIX AND NOT APPLE)
    set(GLFW_BUILD_X11 ON)
    set(GLFW_BUILD_WAYLAND ON)
endif()

set(GLFW_USE_HYBRID_HPG OFF)

find_package(Threads REQUIRED)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_subdirectory(src)