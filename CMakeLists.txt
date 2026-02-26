cmake_minimum_required(VERSION 3.30.0..4.3.0)
project(Nostalgia
    VERSION 0.1.5
    LANGUAGES CXX C)

option(BUILD_SHARED_LIBS "Build the shared version of Nostalgia (Linux only)" OFF)
option(NOSTALGIA_GENERATE_COMPILE_COMMANDS "Generate a compile_commands.json file, fix it, and export it to the source directory" OFF)
option(NOSTALGIA_BUILD_EDITOR "Build the editor/testing app" ON)
option(NOSTALGIA_INSTALL "Generate install targets" ON)
option(NOSTALGIA_INCLUDE_GLM_HEADERS "When installing Nostalgia's headers, include Nostalgia/thirdparty/glm" ON)
option(NOSTALGIA_INCLUDE_JOLT_HEADERS "When installing Nostalgia's headers, include Nostalgia/thirdparty/Jolt" ON)

if(MSVC AND BUILD_SHARED_LIBS)
    message(FATAL_ERROR "Sorry, but building Nostalgia as a DLL is not supported yet; feel free to try and do it yourself, though (and if you can get it working, make a pull request!)")
endif()

if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ${NOSTALGIA_GENERATE_COMPILE_COMMANDS})
endif()

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json)
        include(FixCompileCommands.cmake)
        fix_compile_commands()
    else()
        message(STATUS "NOTE:: If you want to fix cmake's \"compile_commands.json\" file (and copy it to the source directory) please re-run the previous cmake command (or press either \"configure\" or \"generate\" if using cmake-gui)")
    endif()
endif()

if(MSVC)
    set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_DX12)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    message(STATUS "Available Configurations: Debug, Release")
else()
    set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_VK)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
    endif()
    message(STATUS "Build type is '${CMAKE_BUILD_TYPE}'")
endif()

set(NOSTALGIA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Nostalgia)
set(NOSTALGIA_PCH_SRC ${NOSTALGIA_ROOT}/Nostalgia.hpp)

add_custom_target(EmbedAssets
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE_DIR=${NOSTALGIA_ROOT}/embedded
        -P ${NOSTALGIA_ROOT}/embedded/EmbedAssets.cmake)

include(Sources.cmake)
include(Thirdparty.cmake)

add_library(Nostalgia ${NOSTALGIA_SRC_FILES} ${NOSTALGIA_OBJ_FILES} ${NOSTALGIA_HEADERS})
add_dependencies(Nostalgia
    EmbedAssets
    freetype-object
    zlib-object
    bz2-object
    brotlicommon-object
    brotlidec-object
    brotlienc-object
    png-object
    glfw-object
    Jolt-object)
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

if(MSVC)
    target_compile_features(Nostalgia PUBLIC cxx_std_23)
    set_property(TARGET Nostalgia PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET Jolt PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    target_compile_features(Nostalgia PUBLIC cxx_std_26)
endif()

if(MINGW)
    # std:: print, println, ...
    target_link_libraries(Nostalgia PUBLIC stdc++exp)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(Nostalgia PUBLIC NOSTALGIA_DEBUGGING)
    if(NOT MSVC AND ${CMAKE_CXX_COMPILER_ID} EQUAL Clang)
        target_compile_options(Nostalgia PUBLIC -Wall -O0 -g -fsanitize=address)
        target_link_options(Nostalgia PUBLIC -Wall -O0 -g -fsanitize=address)
    endif()
endif()

if(NOSTALGIA_INCLUDE_GLM_HEADERS)
    set(NOSTALGIA_HEADERS ${NOSTALGIA_HEADERS} ${NOSTALGIA_GLM_HEADERS})
endif()

if(NOSTALGIA_INCLUDE_JOLT_HEADERS)
    set(NOSTALGIA_HEADERS ${NOSTALGIA_HEADERS} ${NOSTALGIA_JOLT_HEADERS})
endif()

if(NOSTALGIA_INSTALL)
    # Stolen from JoltPhysics/Build/CMakeLists.txt:338-345
    foreach(HEADER_FILE ${NOSTALGIA_HEADERS})
        string(REPLACE ${NOSTALGIA_ROOT} "Nostalgia" RELATIVE_HEADER_FILE ${HEADER_FILE})
        get_filename_component(DESTINATION_PATH ${RELATIVE_HEADER_FILE} DIRECTORY)
        cmake_path(SET DST_FILE NORMALIZE "${CMAKE_INSTALL_INCLUDEDIR}/${DESTINATION_PATH}")
        install(FILES ${HEADER_FILE} DESTINATION ${DST_FILE})
    endforeach()
    # Install the engine last; this lets users install headers without building the library
    # it also lets the more important build message show up at the bottom.
    if(WIN32)
        install(TARGETS Nostalgia)
        set(_install_dir ${CMAKE_INSTALL_PREFIX})
    else()
        install(TARGETS Nostalgia DESTINATION lib)
        set(_install_dir ${CMAKE_INSTALL_PREFIX}/lib)
    endif()
    message(STATUS "Library Install Dir: ${_install_dir}")
    message(STATUS "Headers Install Dir: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()

################
#### EDITOR ####
################
set(EDITOR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Editor)

if(NOSTALGIA_BUILD_EDITOR)
    # Link the library specific to the current config
    if(BUILD_SHARED_LIBS)
        set(NOSTALGIA_LIB ${CMAKE_SHARED_LIBRARY_PREFIX}Nostalgia${CMAKE_SHARED_LIBRARY_SUFFIX})
    else()
        set(NOSTALGIA_LIB ${CMAKE_STATIC_LIBRARY_PREFIX}Nostalgia${CMAKE_STATIC_LIBRARY_SUFFIX})
    endif()

    add_executable(NostalgiaGoggles ${EDITOR_SRC_FILES})
    add_dependencies(NostalgiaGoggles Nostalgia)
    target_include_directories(NostalgiaGoggles
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${EDITOR_ROOT}/thirdparty/freetype
        PUBLIC ${EDITOR_ROOT} ${EDITOR_ROOT}/thirdparty ${NOSTALGIA_ROOT}/thirdparty ${NOSTALGIA_ROOT}/thirdparty/glfw/include)
    target_link_libraries(NostalgiaGoggles PUBLIC ${NOSTALGIA_LIB})

    if(MSVC)
        target_compile_features(NostalgiaGoggles PUBLIC cxx_std_23)
        set_property(TARGET NostalgiaGoggles PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_compile_features(NostalgiaGoggles PUBLIC cxx_std_26)
    endif()

    if(MINGW)
        target_link_libraries(NostalgiaGoggles PUBLIC stdc++exp)
    endif()

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_definitions(NostalgiaGoggles PUBLIC NOSTALGIA_DEBUGGING)
        if(NOT MSVC AND ${CMAKE_CXX_COMPILER_ID} EQUAL Clang)
            target_compile_options(NostalgiaGoggles PUBLIC -Wall -O0 -g -fsanitize=address)
            target_link_options(NostalgiaGoggles PUBLIC -Wall -O0 -g -fsanitize=address)
        endif()
    endif()
endif()
