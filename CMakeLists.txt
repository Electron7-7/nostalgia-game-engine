cmake_minimum_required(VERSION 3.30.0)
project(Nostalgia
    VERSION 0.1.0
    LANGUAGES CXX C)

option(BUILD_SHARED_LIBS "Build the shared version of Nostalgia" OFF)

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

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(NOSTALGIA_EXTERN_DIR_VER debug)
    set(NOSTALGIA_VERSION_DEFINITION NOSTALGIA_DEBUGGING)
    set(NOSTALGIA_JPH_DEFINITIONS
        JPH_PROFILE_ENABLED JPH_DEBUG_RENDERER JPH_OBJECT_STREAM)
else()
    set(NOSTALGIA_EXTERN_DIR_VER release)
    set(NOSTALGIA_VERSION_DEFINITION NOSTALGIA_RELEASE)
    set(NOSTALGIA_JPH_DEFINITIONS JPH_OBJECT_STREAM)
endif()

if(WIN32)
    set(NOSTALGIA_EXTERN_DIR ${NOSTALGIA_ROOT}/static_libraries/windows/${NOSTALGIA_EXTERN_DIR_VER})
else()
    set(NOSTALGIA_EXTERN_DIR ${NOSTALGIA_ROOT}/static_libraries/linux/${NOSTALGIA_EXTERN_DIR_VER})
    # https://stackoverflow.com/a/14200728
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libz.o
        COMMAND ${CMAKE_CXX_COMPILER} -r -o ${CMAKE_CURRENT_BINARY_DIR}/libz.o -Xlinker --whole-archive ${NOSTALGIA_EXTERN_DIR}/libz.a)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libbz2_static.o
        COMMAND ${CMAKE_CXX_COMPILER} -r -o ${CMAKE_CURRENT_BINARY_DIR}/libbz2_static.o -Xlinker --whole-archive ${NOSTALGIA_EXTERN_DIR}/libbz2_static.a)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libbrotlicommon.o
        COMMAND ${CMAKE_CXX_COMPILER} -r -o ${CMAKE_CURRENT_BINARY_DIR}/libbrotlicommon.o -Xlinker --whole-archive ${NOSTALGIA_EXTERN_DIR}/libbrotlicommon.a)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libbrotlidec.o
        COMMAND ${CMAKE_CXX_COMPILER} -r -o ${CMAKE_CURRENT_BINARY_DIR}/libbrotlidec.o -Xlinker --whole-archive ${NOSTALGIA_EXTERN_DIR}/libbrotlidec.a)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libbrotlienc.o
        COMMAND ${CMAKE_CXX_COMPILER} -r -o ${CMAKE_CURRENT_BINARY_DIR}/libbrotlienc.o -Xlinker --whole-archive ${NOSTALGIA_EXTERN_DIR}/libbrotlienc.a)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libpng18.o
        COMMAND ${CMAKE_CXX_COMPILER} -r -o ${CMAKE_CURRENT_BINARY_DIR}/libpng18.o -Xlinker --whole-archive ${NOSTALGIA_EXTERN_DIR}/libpng18.a)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libfreetype.o
        COMMAND ${CMAKE_CXX_COMPILER} -r -o ${CMAKE_CURRENT_BINARY_DIR}/libfreetype.o -Xlinker --whole-archive ${NOSTALGIA_EXTERN_DIR}/libfreetype.a)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libJolt.o
        COMMAND ${CMAKE_CXX_COMPILER} -r -o ${CMAKE_CURRENT_BINARY_DIR}/libJolt.o -Xlinker --whole-archive ${NOSTALGIA_EXTERN_DIR}/libJolt.a)
    set(NOSTALGIA_OBJ_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libz.o
        ${CMAKE_CURRENT_BINARY_DIR}/libbz2_static.o
        ${CMAKE_CURRENT_BINARY_DIR}/libbrotlicommon.o
        ${CMAKE_CURRENT_BINARY_DIR}/libbrotlidec.o
        ${CMAKE_CURRENT_BINARY_DIR}/libbrotlienc.o
        ${CMAKE_CURRENT_BINARY_DIR}/libpng18.o
        ${CMAKE_CURRENT_BINARY_DIR}/libfreetype.o
        ${CMAKE_CURRENT_BINARY_DIR}/libJolt.o)
endif()

add_library(Nostalgia ${LIBRARY_VERSION} ${NOSTALGIA_SRC_FILES} ${NOSTALGIA_OBJ_FILES})
target_compile_features(Nostalgia PUBLIC cxx_std_26)
target_compile_definitions(Nostalgia PUBLIC
    ${NOSTALGIA_VERSION_DEFINITION}
    ${NOSTALGIA_JPH_DEFINITIONS})
if(${CMAKE_CXX_COMPILER_ID} EQUAL GNU)
    target_compile_definitions(Nostalgia PUBLIC NOSTALGIA_GCC_PCH)
endif()
target_precompile_headers(Nostalgia PUBLIC
    "$<$<COMPILE_LANGUAGE:CXX>:${NOSTALGIA_PCH_SRC}>"
    "$<$<COMPILE_LANGUAGE:C>:<stddef.h$<ANGLE-R>>")
target_include_directories(Nostalgia PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${NOSTALGIA_ROOT}
    ${NOSTALGIA_ROOT}/thirdparty
    ${NOSTALGIA_ROOT}/thirdparty/freetype/include)
target_link_libraries(Nostalgia PUBLIC glfw vulkan)

if(WIN32)
    message(WIN32)
    target_link_directories(Nostalgia PUBLIC ${NOSTALGIA_EXTERN_DIR})
    target_link_libraries(Nostalgia PUBLIC
        libz.lib
        libbz2_static.lib
        libbrotlicommon.lib
        libbrotlidec.lib
        libbrotlienc.lib
        libpng18.lib
        libfreetype.lib
        libJolt.lib)
endif()

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

add_executable(NostalgiaGoggles ${EDITOR_SRC_FILES})
target_compile_features(NostalgiaGoggles PUBLIC cxx_std_26)
target_include_directories(NostalgiaGoggles
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${EDITOR_ROOT})
target_link_directories(NostalgiaGoggles PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(NostalgiaGoggles PUBLIC glfw vulkan getargs ${EDITOR_ENGINE_LIBRARY})

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
    if(UNIX)
        if(${CMAKE_CXX_COMPILER_ID} EQUAL GNU)
            target_compile_options(Nostalgia PUBLIC "-Wall -O0 -g -fsanitize=address")
            target_link_options(Nostalgia PUBLIC "-Wall -O0 -g -fsanitize=address")
            target_compile_options(NostalgiaGoggles PUBLIC "-Wall -O0 -g -fsanitize=address")
            target_link_options(NostalgiaGoggles PUBLIC "-Wall -O0 -g -fsanitize=address")
        endif()
    endif()
endif()
