cmake_minimum_required(VERSION 3.30.0..4.3.0)
project(Nostalgia
    VERSION 0.1.1
    LANGUAGES CXX C)

option(BUILD_SHARED_LIBS "Build the shared version of Nostalgia" OFF)
option(NOSTALGIA_INCLUDE_GLM_HEADERS "When installing Nostalgia's headers, include Nostalgia/thirdparty/glm" ON)
option(NOSTALGIA_INCLUDE_JOLT_HEADERS "When installing Nostalgia's headers, include Nostalgia/thirdparty/Jolt" ON)

set(NOSTALGIA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Nostalgia")
set(NOSTALGIA_PCH_SRC ${NOSTALGIA_ROOT}/Nostalgia.hpp)

# taken from brotli
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to Release as none was specified")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
else()
    message(STATUS "Build type is '${CMAKE_BUILD_TYPE}'")
endif()

if(MSVC)
    set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_DX12)
else()
    set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_VK)
endif()

add_custom_target(EmbedAssets
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE_DIR=${NOSTALGIA_ROOT}/embedded
        -P ${NOSTALGIA_ROOT}/embedded/EmbedAssets.cmake)

include(Sources.cmake)
include(Thirdparty.cmake)

add_library(Nostalgia ${LIBRARY_VERSION} ${NOSTALGIA_SRC_FILES} ${NOSTALGIA_OBJ_FILES})
add_dependencies(Nostalgia EmbedAssets)
target_compile_definitions(Nostalgia PUBLIC
    ${NOSTALGIA_VERSION_DEFINITION}
    ${NOSTALGIA_JPH_DEFINITIONS})
target_link_libraries(Nostalgia PRIVATE
    freetype-object
    zlib-object
    bz2-object
    brotlicommon-object
    brotlidec-object
    brotlienc-object
    png-object
    glfw-object
    Jolt-object)

target_precompile_headers(Nostalgia PUBLIC
    "$<$<COMPILE_LANGUAGE:CXX>:${NOSTALGIA_PCH_SRC}>"
    "$<$<COMPILE_LANGUAGE:C>:<stddef.h$<ANGLE-R>>")
target_include_directories(Nostalgia
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC ${NOSTALGIA_ROOT}
    PRIVATE ${NOSTALGIA_ROOT}/thirdparty
    PRIVATE ${NOSTALGIA_ROOT}/thirdparty/glfw/include
    PRIVATE ${NOSTALGIA_ROOT}/thirdparty/freetype/include)

if(NOSTALGIA_INCLUDE_GLM_HEADERS)
    set(NOSTALGIA_HEADERS ${NOSTALGIA_HEADERS} ${NOSTALGIA_GLM_HEADERS})
endif()

if(NOSTALGIA_INCLUDE_JOLT_HEADERS)
    set(NOSTALGIA_HEADERS ${NOSTALGIA_HEADERS} ${NOSTALGIA_JOLT_HEADERS})
endif()

if(UNIX)
    install(TARGETS Nostalgia DESTINATION lib)

    # Stolen from JoltPhysics/Build/CMakeLists.txt:338-345
    foreach(HEADER_FILE ${NOSTALGIA_HEADERS})
        string(REPLACE ${NOSTALGIA_ROOT} "Nostalgia" RELATIVE_HEADER_FILE ${HEADER_FILE})
        get_filename_component(DESTINATION_PATH ${RELATIVE_HEADER_FILE} DIRECTORY)
        cmake_path(SET DST_FILE NORMALIZE "${CMAKE_INSTALL_INCLUDEDIR}/${DESTINATION_PATH}")
        install(FILES ${HEADER_FILE} DESTINATION ${DST_FILE})
    endforeach()
endif()


################
#### EDITOR ####
################
set(EDITOR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Editor)

if(BUILD_SHARED_LIBS)
    set(EDITOR_ENGINE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Nostalgia${CMAKE_SHARED_LIBRARY_SUFFIX})
else()
    set(EDITOR_ENGINE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Nostalgia${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

if(WIN32 OR MINGW)
    set(EDITOR_LIBRARIES_DIR ${EDITOR_ROOT}/libraries/windows)
else()
    set(EDITOR_LIBRARIES_DIR ${EDITOR_ROOT}/libraries/linux)
endif()

add_executable(NostalgiaGoggles ${EDITOR_SRC_FILES})
target_include_directories(NostalgiaGoggles
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${EDITOR_ROOT}/thirdparty/freetype
    PUBLIC ${EDITOR_ROOT} ${EDITOR_ROOT}/thirdparty ${NOSTALGIA_ROOT}/thirdparty/JoltPhysics)
target_link_directories(NostalgiaGoggles PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${EDITOR_LIBRARIES_DIR})
target_link_libraries(NostalgiaGoggles PUBLIC ${EDITOR_ENGINE_LIBRARY} getargs)

if(MSVC)
    target_compile_features(Nostalgia PUBLIC cxx_std_23)
    target_compile_features(NostalgiaGoggles PUBLIC cxx_std_23)
    set_property(TARGET Nostalgia PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET NostalgiaGoggles PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET Jolt PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    target_compile_features(Nostalgia PUBLIC cxx_std_26)
    target_compile_features(NostalgiaGoggles PUBLIC cxx_std_26)
endif()

if(MINGW)
    # std:: print, println, ...
    target_link_libraries(Nostalgia PUBLIC stdc++exp)
    target_link_libraries(NostalgiaGoggles PUBLIC stdc++exp)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    if(NOT MSVC AND ${CMAKE_CXX_COMPILER_ID} EQUAL Clang)
        target_compile_options(Nostalgia PUBLIC -Wall -O0 -g -fsanitize=address)
        target_link_options(Nostalgia PUBLIC -Wall -O0 -g -fsanitize=address)
        target_compile_options(NostalgiaGoggles PUBLIC -Wall -O0 -g -fsanitize=address)
        target_link_options(NostalgiaGoggles PUBLIC -Wall -O0 -g -fsanitize=address)
    endif()
endif()
