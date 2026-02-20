cmake_minimum_required(VERSION 3.30.0)
project(Nostalgia
    VERSION 0.1.0
    LANGUAGES CXX C)

option(BUILD_SHARED_LIBS "Build the shared version of Nostalgia" OFF)

if(WIN32 OR ${CMAKE_SYSTEM_NAME} EQUAL Windows)
    set(IS_WINDOWS ON)
else()
    set(IS_WINDOWS OFF)
endif()

set(NOSTALGIA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Nostalgia")

set(NOSTALGIA_PCH_SRC ${NOSTALGIA_ROOT}/Nostalgia.hpp)
set(NOSTALGIA_SRC_FILES
    ${NOSTALGIA_ROOT}/backends/opengl/gl_graphics_context.cpp
    ${NOSTALGIA_ROOT}/backends/opengl/gl_mesh_buffers.cpp
    ${NOSTALGIA_ROOT}/backends/opengl/gl_vertex_array.cpp
    ${NOSTALGIA_ROOT}/backends/opengl/gl_texture_buffer.cpp
    ${NOSTALGIA_ROOT}/backends/opengl/gl_shader.cpp
    ${NOSTALGIA_ROOT}/backends/opengl/gl_renderer_api.cpp
    ${NOSTALGIA_ROOT}/backends/opengl/gl_frame_buffer.cpp
    ${NOSTALGIA_ROOT}/backends/glfw/glfw_window.cpp
    ${NOSTALGIA_ROOT}/backends/imgui/imgui_implementor.cpp
    ${NOSTALGIA_ROOT}/managers/event_manager.cpp
    ${NOSTALGIA_ROOT}/managers/physics_manager.cpp
    ${NOSTALGIA_ROOT}/managers/input_manager.cpp
    ${NOSTALGIA_ROOT}/managers/render_manager.cpp
    ${NOSTALGIA_ROOT}/managers/theatre_manager.cpp
    ${NOSTALGIA_ROOT}/managers/manager.cpp
    ${NOSTALGIA_ROOT}/managers/ui_manager.cpp
    ${NOSTALGIA_ROOT}/settings/world.cpp
    ${NOSTALGIA_ROOT}/core/uid.cpp
    ${NOSTALGIA_ROOT}/core/time.cpp
    ${NOSTALGIA_ROOT}/core/message_labels.cpp
    ${NOSTALGIA_ROOT}/core/bitmask.cpp
    ${NOSTALGIA_ROOT}/core/ansi_sequence.cpp
    ${NOSTALGIA_ROOT}/core/log.cpp
    ${NOSTALGIA_ROOT}/core/id.cpp
    ${NOSTALGIA_ROOT}/application/application.cpp
    ${NOSTALGIA_ROOT}/application/window.cpp
    ${NOSTALGIA_ROOT}/events/action.cpp
    ${NOSTALGIA_ROOT}/events/event_queue.cpp
    ${NOSTALGIA_ROOT}/events/event.cpp
    ${NOSTALGIA_ROOT}/ui/implementor.cpp
    ${NOSTALGIA_ROOT}/ui/solution.cpp
    ${NOSTALGIA_ROOT}/theatre/theatre.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thing.cpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/material.cpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/font.cpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/mesh.cpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/texture.cpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/resource.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/light_3d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/visual_3d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/mesh_instance_3d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/collider_3d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/actor_3d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/camera_3d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/nostalgia_player_3d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/camera_2d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/actor_2d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/sprite_2d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/text_2d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/visual_2d.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/viewport.cpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/thinker.cpp
    ${NOSTALGIA_ROOT}/theatre/thing_data.cpp
    ${NOSTALGIA_ROOT}/theatre/thing_variable.cpp
    ${NOSTALGIA_ROOT}/theatre/parser/parser.cpp
    ${NOSTALGIA_ROOT}/theatre/parser/lexer.cpp
    ${NOSTALGIA_ROOT}/theatre/thing_type.cpp
    ${NOSTALGIA_ROOT}/theatre/variable_registry.cpp
    ${NOSTALGIA_ROOT}/theatre/thing_factory.cpp
    ${NOSTALGIA_ROOT}/theatre/call_sheet.cpp
    ${NOSTALGIA_ROOT}/theatre/theatre_file.cpp
    ${NOSTALGIA_ROOT}/theatre/tree.cpp
    ${NOSTALGIA_ROOT}/filesystem/file_data.cpp
    ${NOSTALGIA_ROOT}/filesystem/filesystem.cpp
    ${NOSTALGIA_ROOT}/physics/engine.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui_impl_opengl3.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui_widgets.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui_stdlib.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui_tables.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui_impl_glfw.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui_demo.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui_freetype.cpp
    ${NOSTALGIA_ROOT}/thirdparty/DearImGui/imgui_draw.cpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/glm.cpp
    ${NOSTALGIA_ROOT}/thirdparty/glad/glad.c
    ${NOSTALGIA_ROOT}/rendering/texture_buffer.cpp
    ${NOSTALGIA_ROOT}/rendering/shader.cpp
    ${NOSTALGIA_ROOT}/rendering/renderer_api.cpp
    ${NOSTALGIA_ROOT}/rendering/graphics_context.cpp
    ${NOSTALGIA_ROOT}/rendering/vertex_array.cpp
    ${NOSTALGIA_ROOT}/rendering/mesh_buffers.cpp
    ${NOSTALGIA_ROOT}/rendering/frame_buffer.cpp
    ${NOSTALGIA_ROOT}/console/console.cpp
    ${NOSTALGIA_ROOT}/math/conversion.cpp
)

# taken from brotli
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to Release as none was specified")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
else()
    message(STATUS "Build type is '${CMAKE_BUILD_TYPE}'")
endif()

set(THIRDPARTY_DIR ${NOSTALGIA_ROOT}/thirdparty)

add_subdirectory(${THIRDPARTY_DIR}/zlib)

set(ENABLE_SHARED_LIB OFF)
set(ENABLE_STATIC_LIB OFF)
add_subdirectory(${THIRDPARTY_DIR}/bzip2)

set(BROTLI_BUNDLED_MODE OFF)
add_subdirectory(${THIRDPARTY_DIR}/brotli)

add_subdirectory(${THIRDPARTY_DIR}/libpng)

set(FT_DISABLE_ZLIB ON)
set(FT_DISABLE_BZIP2 ON)
set(FT_DISABLE_PNG ON)
set(FT_DISABLE_HARFBUZZ ON)
set(FT_DISABLE_BROTLI ON)
add_subdirectory(${THIRDPARTY_DIR}/freetype)

set(GLFW_LIBRARY_TYPE OBJECT)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
add_subdirectory(${THIRDPARTY_DIR}/glfw)

# For now, I'll be using the debug version of Jolt for both build versions
set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_VK)

set(TARGET_UNIT_TESTS OFF)
set(TARGET_HELLO_WORLD OFF)
set(TARGET_PERFORMANCE_TEST OFF)
set(TARGET_SAMPLES OFF)
set(TARGET_VIEWER OFF)
set(JPH_USE_DX12 OFF)
set(JPH_USE_DXC OFF)
set(JPH_USE_CPU_COMPUTE OFF)
set(ENABLE_INSTALL OFF)
set(ENABLE_OBJECT_STREAM ON)
set(DEBUG_RENDERER_IN_DEBUG_AND_RELEASE OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY ON)
add_subdirectory(${THIRDPARTY_DIR}/JoltPhysics/Build)

set(EMBEDDED_ASSETS fonts.hpp images.hpp models.hpp shaders.hpp)

add_custom_target(EmbedAssets
    COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${NOSTALGIA_ROOT}/embedded -P ${NOSTALGIA_ROOT}/embedded/EmbedAssets.cmake)

add_library(Nostalgia ${LIBRARY_VERSION} ${NOSTALGIA_SRC_FILES} ${NOSTALGIA_OBJ_FILES})
if(IS_WINDOWS)
    target_compile_features(Nostalgia PUBLIC cxx_std_23)
else()
    target_compile_features(Nostalgia PUBLIC cxx_std_26)
endif()
add_dependencies(Nostalgia EmbedAssets)
target_compile_definitions(Nostalgia PUBLIC
    ${NOSTALGIA_VERSION_DEFINITION}
    ${NOSTALGIA_JPH_DEFINITIONS})
target_link_libraries(Nostalgia PRIVATE
    freetype_object
    zlib_object
    bz2_ObjLib
    brotlicommon-object
    brotlidec-object
    brotlienc-object
    png_object
    glfw_object
    Jolt_object)

if(${CMAKE_CXX_COMPILER_ID} EQUAL GNU OR ${CMAKE_CXX_COMPILER_ID} EQUAL MSVC)
    target_compile_definitions(Nostalgia PUBLIC NOSTALGIA_GCC_PCH)
endif()

target_precompile_headers(Nostalgia PUBLIC
    "$<$<COMPILE_LANGUAGE:CXX>:${NOSTALGIA_PCH_SRC}>"
    "$<$<COMPILE_LANGUAGE:C>:<stddef.h$<ANGLE-R>>")
target_include_directories(Nostalgia PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${NOSTALGIA_ROOT}
    ${NOSTALGIA_ROOT}/thirdparty
    ${NOSTALGIA_ROOT}/thirdparty/JoltPhysics
    ${NOSTALGIA_ROOT}/thirdparty/glfw/include
    ${NOSTALGIA_ROOT}/thirdparty/freetype/include)


################
#### EDITOR ####
################
set(EDITOR_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Editor)
set(EDITOR_SRC_FILES
    ${EDITOR_ROOT}/things/player.cpp
    ${EDITOR_ROOT}/app/nostalgia_goggles.cpp
    ${EDITOR_ROOT}/system/dedicated_main.cpp
    ${EDITOR_ROOT}/tools/stopwatch_log.cpp
    ${EDITOR_ROOT}/gui/imgui_debugger.cpp
    ${EDITOR_ROOT}/gui/imgui_editor.cpp)

if(BUILD_SHARED_LIBS)
    set(EDITOR_ENGINE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Nostalgia${CMAKE_SHARED_LIBRARY_SUFFIX})
else()
    set(EDITOR_ENGINE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}Nostalgia${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

if(IS_WINDOWS)
    set(EDITOR_LIBRARIES_DIR ${EDITOR_ROOT}/libraries/windows)
else()
    set(EDITOR_LIBRARIES_DIR ${EDITOR_ROOT}/libraries/linux)
endif()

add_executable(NostalgiaGoggles ${EDITOR_SRC_FILES})
if(IS_WINDOWS)
    target_compile_features(NostalgiaGoggles PUBLIC cxx_std_23)
else()
    target_compile_features(NostalgiaGoggles PUBLIC cxx_std_26)
endif()
target_include_directories(NostalgiaGoggles
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${EDITOR_ROOT} ${EDITOR_ROOT}/thirdparty ${NOSTALGIA_ROOT}/thirdparty/JoltPhysics)
target_link_directories(NostalgiaGoggles PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${EDITOR_LIBRARIES_DIR})
target_link_libraries(NostalgiaGoggles PUBLIC ${EDITOR_ENGINE_LIBRARY} getargs)

if(MSVC)
    set_property(TARGET Nostalgia PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET NostalgiaGoggles PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET Jolt PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    if(NOT IS_WINDOWS AND ${CMAKE_CXX_COMPILER_ID} EQUAL Clang)
        target_compile_options(Nostalgia PUBLIC "-Wall -O0 -g -fsanitize=address")
        target_link_options(Nostalgia PUBLIC "-Wall -O0 -g -fsanitize=address")
        target_compile_options(NostalgiaGoggles PUBLIC "-Wall -O0 -g -fsanitize=address")
        target_link_options(NostalgiaGoggles PUBLIC "-Wall -O0 -g -fsanitize=address")
    endif()
endif()

if(MINGW)
    # std:: print, println, ...
    target_link_libraries(Nostalgia PUBLIC stdc++exp)
    target_link_libraries(NostalgiaGoggles PUBLIC stdc++exp)
endif()
