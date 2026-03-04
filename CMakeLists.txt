cmake_minimum_required(VERSION 3.30.0..4.3.0)
project(Nostalgia
    VERSION 0.1.5
    LANGUAGES CXX C)

option(BUILD_SHARED_LIBS "Build the shared version of Nostalgia (Linux only)" OFF)
option(NOSTALGIA_GENERATE_COMPILE_COMMANDS "Generate a compile_commands.json file, fix it, and export it to the source directory" OFF)
option(BUILD_EDITOR "Build the editor/testing app" ON)
option(NOSTALGIA_INSTALL "Generate install targets" ON)
option(NOSTALGIA_INCLUDE_GLM_HEADERS "When installing Nostalgia's headers, include Nostalgia/thirdparty/glm" ON)
option(NOSTALGIA_INCLUDE_JOLT_HEADERS "When installing Nostalgia's headers, include Nostalgia/thirdparty/Jolt" ON)
if(NOT MSVC AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    option(NOSTALGIA_USE_ADDRESS_SANITIZER "Use address sanitizer when building Nostalgia and the included testing apps" OFF)
endif()

# Testapp Options
option(BUILD_ALL_TESTS "Build all test apps (overrides individual testapp building options" ON)
option(BUILD_NO_THIRDPARTY_TEST "Build 'ThirdpartyTestApp_NoThirdparty'" OFF)
option(BUILD_JOLT_THIRDPARTY_TEST "Build 'ThirdpartyTestApp_JoltPhysics'" OFF)

if(BUILD_ALL_TESTS)
    set(BUILD_NO_THIRDPARTY_TEST ON)
    set(BUILD_JOLT_THIRDPARTY_TEST ON)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Append this to the debug library's name")
endif()

if(MSVC)
    if(BUILD_SHARED_LIBS)
        message(FATAL_ERROR "Sorry, but building Nostalgia as a DLL is not supported yet; feel free to try and do it yourself, though (and if you can get it working, make a pull request!)")
    endif()

    set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_DX12)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_CURRENT_BINARY_DIR}/NostalgiaHeaderFiles)
    message(STATUS "Available Configurations: Debug, Release")
else()
    set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_VK)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
    endif()
    message(STATUS "Build type is '${CMAKE_BUILD_TYPE}'")
    if(BUILD_SHARED_LIBS)
        set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif()
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

set(NOSTALGIA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Nostalgia)
set(NOSTALGIA_PCH_SRC ${NOSTALGIA_ROOT}/Nostalgia.hpp)
set(NOSTALGIA_EMBED_FILES fonts.hpp images.hpp models.hpp shaders.hpp)

add_custom_command(OUTPUT ${NOSTALGIA_EMBED_FILES}
    COMMAND ${CMAKE_COMMAND}
    ARGS -DSOURCE_DIR=${NOSTALGIA_ROOT}/embedded -P ${NOSTALGIA_ROOT}/embedded/EmbedAssets.cmake)

include(Sources.cmake)
include(Thirdparty.cmake)

add_library(Nostalgia ${NOSTALGIA_SRC_FILES} ${NOSTALGIA_OBJ_FILES} ${NOSTALGIA_HEADERS} ${NOSTALGIA_EMBED_FILES})
add_dependencies(Nostalgia
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
    ${NOSTALGIA_JPH_DEFINITIONS}
    $<$<CONFIG:Debug>:NOSTALGIA_DEBUGGING>)
target_compile_options(Nostalgia
    PUBLIC $<$<CONFIG:Debug>:-Wall -O0>
    PUBLIC $<$<BOOL:${NOSTALGIA_USE_ADDRESS_SANITIZER}>:-g -fsanitize=address>)
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
    if(NOT MSVC)
        install(TARGETS Nostalgia DESTINATION lib)
    endif()
endif()

################
#### EDITOR ####
################
set(EDITOR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Editor)

if(BUILD_EDITOR)
    add_executable(NostalgiaGoggles ${EDITOR_SRC_FILES})
    add_dependencies(NostalgiaGoggles Nostalgia)
    target_compile_definitions(NostalgiaGoggles PUBLIC $<$<CONFIG:Debug>:NOSTALGIA_DEBUGGING>)
    target_compile_options(NostalgiaGoggles
        PUBLIC $<$<CONFIG:Debug>:-Wall -O0>
        PUBLIC $<$<BOOL:${NOSTALGIA_USE_ADDRESS_SANITIZER}>:-g -fsanitize=address>)
    target_include_directories(NostalgiaGoggles
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${EDITOR_ROOT}/thirdparty/freetype
        PUBLIC ${EDITOR_ROOT} ${EDITOR_ROOT}/thirdparty ${NOSTALGIA_ROOT}/thirdparty ${NOSTALGIA_ROOT}/thirdparty/glfw/include)
    target_link_directories(NostalgiaGoggles PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    target_link_libraries(NostalgiaGoggles PUBLIC $<TARGET_FILE:Nostalgia>)

    if(MSVC)
        target_compile_features(NostalgiaGoggles PUBLIC cxx_std_23)
        set_property(TARGET NostalgiaGoggles PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_compile_features(NostalgiaGoggles PUBLIC cxx_std_26)
    endif()
endif()

##############
## TESTAPPS ##
##############
set(TESTAPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Tests)
set(TESTAPP_MAIN ${TESTAPP_DIR}/shared_main.cpp)

function(add_test_target TARGET_NAME TARGET_SOURCES)
    add_executable(${TARGET_NAME} ${TESTAPP_MAIN} ${TARGET_SOURCES})
    add_dependencies(${TARGET_NAME} Nostalgia)
    target_link_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>)
    target_link_libraries(${TARGET_NAME} PUBLIC $<TARGET_FILE:Nostalgia>)
    target_compile_definitions(${TARGET_NAME}
        PUBLIC $<$<CONFIG:Debug>:NOSTALGIA_DEBUGGING>)
    target_compile_options(${TARGET_NAME}
        PUBLIC $<$<CONFIG:Debug>:-Wall -O0>
        PUBLIC $<$<BOOL:${NOSTALGIA_USE_ADDRESS_SANITIZER}>:-g -fsanitize=address>)
    if(MSVC)
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${NOSTALGIA_ROOT}/thirdparty)
        target_compile_features(${TARGET_NAME} PUBLIC cxx_std_23)
        set_property(TARGET ${TARGET_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_compile_features(${TARGET_NAME} PUBLIC cxx_std_26)
    endif()
endfunction()

if(BUILD_NO_THIRDPARTY_TEST)
    add_test_target(ThirdpartyTestApp_NoThirdparty ${TESTAPP_DIR}/Thirdparty/NoThirdparty/app.cpp)
endif()

if(BUILD_JOLT_THIRDPARTY_TEST)
    add_test_target(ThirdpartyTestApp_JoltPhysics ${TESTAPP_DIR}/Thirdparty/JoltPhysics/app.cpp)
endif()

##############
## TESTAPPS ##
##############
set(TESTAPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Tests)
set(TESTAPP_MAIN ${TESTAPP_DIR}/shared_main.cpp)

function(add_test_target TARGET_NAME TARGET_SOURCES)
    add_executable(${TARGET_NAME} ${TESTAPP_MAIN} ${TARGET_SOURCES})
    add_dependencies(${TARGET_NAME} Nostalgia)
    target_link_libraries(${TARGET_NAME} PUBLIC $<TARGET_FILE:Nostalgia>)
    target_compile_definitions(${TARGET_NAME}
        PUBLIC $<$<CONFIG:Debug>:NOSTALGIA_DEBUGGING>)
    target_compile_options(${TARGET_NAME}
        PUBLIC $<$<CONFIG:Debug>:-Wall -O0>
        PUBLIC $<$<BOOL:${NOSTALGIA_USE_ADDRESS_SANITIZER}>:-g -fsanitize=address>)
    if(MSVC)
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${NOSTALGIA_ROOT}/thirdparty)
        target_compile_features(${TARGET_NAME} PUBLIC cxx_std_23)
        set_property(TARGET ${TARGET_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_compile_features(${TARGET_NAME} PUBLIC cxx_std_26)
    endif()
endfunction()

if(BUILD_NO_THIRDPARTY_TEST)
    add_test_target(ThirdpartyTestApp_NoThirdparty ${TESTAPP_DIR}/Thirdparty/NoThirdparty/app.cpp)
endif()

if(BUILD_JOLT_THIRDPARTY_TEST)
    add_test_target(ThirdpartyTestApp_JoltPhysics ${TESTAPP_DIR}/Thirdparty/JoltPhysics/app.cpp)
endif()
