cmake_minimum_required(VERSION 3.30.0..4.3.0)
project(Nostalgia
    VERSION 0.1.5
    LANGUAGES CXX C)

option(BUILD_SHARED_LIBS "Build the shared version of Nostalgia" OFF)
option(NOSTALGIA_GENERATE_COMPILE_COMMANDS "Generate a compile_commands.json file, fix it, and export it to the source directory" OFF)
option(NOSTALGIA_BUILD_EDITOR "Build the editor/testing app" ON)
option(NOSTALGIA_INSTALL "Generate install targets" ON)
option(NOSTALGIA_INCLUDE_GLM_HEADERS "When installing Nostalgia's headers, include Nostalgia/thirdparty/glm" ON)
option(NOSTALGIA_INCLUDE_JOLT_HEADERS "When installing Nostalgia's headers, include Nostalgia/thirdparty/Jolt" ON)
option(NOSTALGIA_ADD_SUFFIX_TO_DEBUG_BUILD "When building the debug version of Nostalgia, name the library 'NostalgiaDebug'" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ${NOSTALGIA_GENERATE_COMPILE_COMMANDS})

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    include(FixCompileCommands.cmake)
    fix_compile_commands()
endif()

if(NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif()
message(STATUS "Build type is '${CMAKE_BUILD_TYPE}'")

if(${NOSTALGIA_ADD_SUFFIX_TO_DEBUG_BUILD} AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(NOSTALGIA_LIB_VERSION_SUFFIX Debug)
endif()

set(NOSTALGIA_LIBRARY_NAME Nostalgia${NOSTALGIA_LIB_VERSION_SUFFIX})
set(NOSTALGIA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Nostalgia)
set(NOSTALGIA_PCH_SRC ${NOSTALGIA_ROOT}/Nostalgia.hpp)

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

add_library(${NOSTALGIA_LIBRARY_NAME} ${LIBRARY_VERSION} ${NOSTALGIA_SRC_FILES} ${NOSTALGIA_OBJ_FILES} ${NOSTALGIA_HEADERS})
add_dependencies(${NOSTALGIA_LIBRARY_NAME}
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
target_compile_definitions(${NOSTALGIA_LIBRARY_NAME} PUBLIC
    ${NOSTALGIA_VERSION_DEFINITION}
    ${NOSTALGIA_JPH_DEFINITIONS})
target_link_libraries(${NOSTALGIA_LIBRARY_NAME} PRIVATE
    freetype-object
    zlib-object
    bz2-object
    brotlicommon-object
    brotlidec-object
    brotlienc-object
    png-object
    glfw-object
    Jolt-object)

target_precompile_headers(${NOSTALGIA_LIBRARY_NAME} PUBLIC
    "$<$<COMPILE_LANGUAGE:CXX>:${NOSTALGIA_PCH_SRC}>"
    "$<$<COMPILE_LANGUAGE:C>:<stddef.h$<ANGLE-R>>")
target_include_directories(${NOSTALGIA_LIBRARY_NAME}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC ${NOSTALGIA_ROOT}
    PRIVATE ${NOSTALGIA_ROOT}/thirdparty
    PRIVATE ${NOSTALGIA_ROOT}/thirdparty/glfw/include
    PRIVATE ${NOSTALGIA_ROOT}/thirdparty/freetype/include)

if(MSVC)
    target_compile_features(${NOSTALGIA_LIBRARY_NAME} PUBLIC cxx_std_23)
    if(${BUILD_SHARED_LIBS})
        set_property(TARGET ${NOSTALGIA_LIBRARY_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        set_property(TARGET Jolt PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        set_property(TARGET ${NOSTALGIA_LIBRARY_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET Jolt PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
else()
    target_compile_features(${NOSTALGIA_LIBRARY_NAME} PUBLIC cxx_std_26)
endif()

if(MINGW)
    # std:: print, println, ...
    target_link_libraries(${NOSTALGIA_LIBRARY_NAME} PUBLIC stdc++exp)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(${NOSTALGIA_LIBRARY_NAME} PUBLIC NOSTALGIA_DEBUGGING)
    if(NOT MSVC AND ${CMAKE_CXX_COMPILER_ID} EQUAL Clang)
        target_compile_options(${NOSTALGIA_LIBRARY_NAME} PUBLIC -Wall -O0 -g -fsanitize=address)
        target_link_options(${NOSTALGIA_LIBRARY_NAME} PUBLIC -Wall -O0 -g -fsanitize=address)
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
        install(TARGETS ${NOSTALGIA_LIBRARY_NAME})
        set(_install_dir ${CMAKE_INSTALL_PREFIX})
    else()
        install(TARGETS ${NOSTALGIA_LIBRARY_NAME} DESTINATION lib)
        set(_install_dir ${CMAKE_INSTALL_PREFIX}/lib)
    endif()
    message(STATUS "Library Install Dir: ${_install_dir}")
    message(STATUS "Headers Install Dir: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()

################
#### EDITOR ####
################
set(EDITOR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Editor)

if(BUILD_SHARED_LIBS)
    set(EDITOR_ENGINE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${NOSTALGIA_LIBRARY_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})
else()
    set(EDITOR_ENGINE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${NOSTALGIA_LIBRARY_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

if(WIN32 OR MINGW)
    set(EDITOR_LIBRARIES_DIR ${EDITOR_ROOT}/libraries/windows)
else()
    set(EDITOR_LIBRARIES_DIR ${EDITOR_ROOT}/libraries/linux)
endif()

if(NOSTALGIA_BUILD_EDITOR)
    add_executable(NostalgiaGoggles ${EDITOR_SRC_FILES})
    add_dependencies(NostalgiaGoggles ${NOSTALGIA_LIBRARY_NAME})
    target_include_directories(NostalgiaGoggles
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${EDITOR_ROOT}/thirdparty/freetype
        PUBLIC ${EDITOR_ROOT} ${EDITOR_ROOT}/thirdparty ${NOSTALGIA_ROOT}/thirdparty ${NOSTALGIA_ROOT}/thirdparty/glfw/include)
    target_link_directories(NostalgiaGoggles PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${EDITOR_LIBRARIES_DIR})
    target_link_libraries(NostalgiaGoggles PUBLIC ${EDITOR_ENGINE_LIBRARY} getargs)

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
