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

set(TARGET_UNIT_TESTS OFF)
set(TARGET_HELLO_WORLD OFF)
set(TARGET_PERFORMANCE_TEST OFF)
set(TARGET_SAMPLES OFF)
set(TARGET_VIEWER OFF)
if(MSVC)
    set(JPH_USE_DX12 ON)
    set(JPH_USE_VK OFF)
    set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_DX12)
else()
    set(JPH_USE_DX12 OFF)
    set(JPH_USE_VK ON)
    set(NOSTALGIA_JPH_DEFINITIONS JPH_PROFILE_ENABLED JPH_USE_VK)
endif()
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
    ${EDITOR_ROOT}/gui/imgui_implementor.cpp
    ${EDITOR_ROOT}/gui/imgui_debugger.cpp
    ${EDITOR_ROOT}/gui/imgui_editor.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui_impl_opengl3.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui_widgets.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui_stdlib.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui_tables.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui_impl_glfw.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui_demo.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui_freetype.cpp
    ${EDITOR_ROOT}/thirdparty/DearImGui/imgui_draw.cpp)

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
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${EDITOR_ROOT}/thirdparty/freetype
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

set(NOSTALGIA_HEADER_FILES
    ${NOSTALGIA_ROOT}/managers/ui_manager.hpp
    ${NOSTALGIA_ROOT}/managers/input_manager.hpp
    ${NOSTALGIA_ROOT}/managers/theatre_manager.hpp
    ${NOSTALGIA_ROOT}/managers/manager.hpp
    ${NOSTALGIA_ROOT}/managers/render_manager.hpp
    ${NOSTALGIA_ROOT}/managers/physics_manager.hpp
    ${NOSTALGIA_ROOT}/managers/event_manager.hpp
    ${NOSTALGIA_ROOT}/settings/player.hpp
    ${NOSTALGIA_ROOT}/settings/engine.hpp
    ${NOSTALGIA_ROOT}/settings/graphics.hpp
    ${NOSTALGIA_ROOT}/settings/world.hpp
    ${NOSTALGIA_ROOT}/core/error.hpp
    ${NOSTALGIA_ROOT}/core/vector.hpp
    ${NOSTALGIA_ROOT}/core/smart_pointers.hpp
    ${NOSTALGIA_ROOT}/core/uid.hpp
    ${NOSTALGIA_ROOT}/core/ansi_sequence.hpp
    ${NOSTALGIA_ROOT}/core/id.hpp
    ${NOSTALGIA_ROOT}/core/ansi_codes.hpp
    ${NOSTALGIA_ROOT}/core/mutex.hpp
    ${NOSTALGIA_ROOT}/core/message_labels.hpp
    ${NOSTALGIA_ROOT}/core/safe_return.hpp
    ${NOSTALGIA_ROOT}/core/enum_prettifier.hpp
    ${NOSTALGIA_ROOT}/core/farg.hpp
    ${NOSTALGIA_ROOT}/core/bitmask.hpp
    ${NOSTALGIA_ROOT}/core/concepts.hpp
    ${NOSTALGIA_ROOT}/core/types.hpp
    ${NOSTALGIA_ROOT}/core/macros.hpp
    ${NOSTALGIA_ROOT}/core/constexpr_string_hash.hpp
    ${NOSTALGIA_ROOT}/core/log.hpp
    ${NOSTALGIA_ROOT}/core/time.hpp
    ${NOSTALGIA_ROOT}/application/window.hpp
    ${NOSTALGIA_ROOT}/application/monitor.hpp
    ${NOSTALGIA_ROOT}/application/application.hpp
    ${NOSTALGIA_ROOT}/events/bindings.hpp
    ${NOSTALGIA_ROOT}/events/action.hpp
    ${NOSTALGIA_ROOT}/events/event_queue.hpp
    ${NOSTALGIA_ROOT}/events/event.hpp
    ${NOSTALGIA_ROOT}/ui/solution.hpp
    ${NOSTALGIA_ROOT}/ui/implementor.hpp
    ${NOSTALGIA_ROOT}/theatre/theatre.hpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/texture.hpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/font.hpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/resource.hpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/mesh.hpp
    ${NOSTALGIA_ROOT}/theatre/things/resources/material.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thing.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/nostalgia_player_3d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/collider_3d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/actor_3d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/visual_3d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/mesh_instance_3d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/camera_3d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/3d/light_3d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/viewport.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/visual_2d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/sprite_2d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/actor_2d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/mesh_instance_2d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/text_2d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/2d/camera_2d.hpp
    ${NOSTALGIA_ROOT}/theatre/things/thinkers/thinker.hpp
    ${NOSTALGIA_ROOT}/theatre/theatre_file.hpp
    ${NOSTALGIA_ROOT}/theatre/tree.hpp
    ${NOSTALGIA_ROOT}/theatre/thing_data.hpp
    ${NOSTALGIA_ROOT}/theatre/call_sheet.hpp
    ${NOSTALGIA_ROOT}/theatre/variable_registry.hpp
    ${NOSTALGIA_ROOT}/theatre/thing_type.hpp
    ${NOSTALGIA_ROOT}/theatre/parser/number_parser.hpp
    ${NOSTALGIA_ROOT}/theatre/thing_factory.hpp
    ${NOSTALGIA_ROOT}/filesystem/file_data.hpp
    ${NOSTALGIA_ROOT}/filesystem/filesystem.hpp
    ${NOSTALGIA_ROOT}/physics/engine.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int3x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float2x3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_uint3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint3x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_clip_space.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_bool4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double3x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_bool4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_reciprocal.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int3x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_packing.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_common.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_exponential.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint4x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float3x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float3x3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_trigonometric.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_ulp.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_bool2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_bool2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint2x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float3x2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float4x4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_int4_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_double1_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float4x3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_int1.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double2x3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int2x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_projection.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint4x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint2x4_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint2x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_uint2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_common.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int4x4_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float4x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float2x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_double_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double4x2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float2x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_uint1_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double2x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_integer.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_reciprocal.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double4x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int4x2_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float4x2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_double4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_integer.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_double3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_common.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int4x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int2x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int3x4_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int4x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double3x3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_uint4_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_double.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint3x4_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_relational.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int4x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint3x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_int_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double2x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int3x3_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_ulp.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double3x4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float4x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double4x4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int2x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float2x4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int2x3_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float3x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_float3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double3x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double4x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double3x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_float3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_double2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_uint2_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_float1_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_bool1.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double4x3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint4x3_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double4x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_relational.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_float1.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int3x2_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_uint1.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_int2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_packing.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint4x2_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_bool1_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint4x4_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_double3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_uint_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_int4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int2x2_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_float4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_float.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float4x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_constants.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_float2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int2x4_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_double2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_common.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_transform.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double2x4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float2x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_int3_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_int1_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint2x2_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_int2_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_geometric.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_float_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint3x2_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_double4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_uint4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float2x2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_bool3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float3x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double2x2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_double1.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/quaternion_transform.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_float3x4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int3x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double2x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_bool3_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint3x3_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_double3x2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint4x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_int3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint2x3_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint2x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_relational.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/_matrix_vectorize.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_uint3x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_float4_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/matrix_int4x3_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_relational.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_uint3_sized.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/scalar_integer.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext/vector_float2_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat2x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/geometric.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat4x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat2x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/vec2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/transform2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/vec_swizzle.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/raw_data.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/color_encoding.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/euler_angles.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/matrix_interpolation.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/polar_coordinates.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/matrix_factorisation.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/matrix_major_storage.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/vector_query.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/gradient_paint.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/texture.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/component_wise.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/fast_exponential.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/fast_square_root.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/norm.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/vector_angle.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/matrix_transform_2d.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/dual_quaternion.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/color_space.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/hash.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/string_cast.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/closest_point.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/normalize_dot.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/mixed_product.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/common.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/quaternion.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/pca.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/std_based_type.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/associated_min_max.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/perpendicular.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/color_space_YCoCg.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/transform.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/projection.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/number_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/extend.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/io.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/integer.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/exterior_product.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/rotate_normalized_axis.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/rotate_vector.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/type_trait.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/type_aligned.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/easing.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/handed_coordinate_space.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/matrix_decompose.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/wrap.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/functions.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/fast_trigonometry.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/intersect.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/compatibility.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/normal.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/matrix_cross_product.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/matrix_operation.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/orthonormalize.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/optimum_pow.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/bit.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/range.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/log_base.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/scalar_relational.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/scalar_multiplication.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/extended_min_max.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/spline.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtx/matrix_query.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/vec4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat2x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/matrix.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/bitfield.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/random.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/vec1.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/round.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/color_space.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/quaternion.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/matrix_integer.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/constants.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/type_precision.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/integer.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/type_aligned.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/type_ptr.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/packing.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/matrix_access.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/reciprocal.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/matrix_transform.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/matrix_inverse.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/ulp.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/noise.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/gtc/epsilon.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat3x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/common.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat3x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat4x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/exponential.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat3x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/integer.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/trigonometric.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/platform.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/matrix.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/exponential.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/packing.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/neon.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/geometric.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/integer.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/common.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/simd/vector_relational.h
    ${NOSTALGIA_ROOT}/thirdparty/glm/glm.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/packing.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/_vectorize.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_vec3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat2x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat4x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/compute_common.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/_swizzle.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat3x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_vec4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/qualifier.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_quat.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/_fixes.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_half.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat4x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/compute_vector_relational.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_float.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_vec2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/_features.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat2x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat2x2.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/compute_vector_decl.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat3x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat3x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/_noise.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_mat4x3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/type_vec1.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/setup.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/detail/_swizzle_func.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/trigonometric.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/fwd.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/mat4x4.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/vector_relational.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/vec3.hpp
    ${NOSTALGIA_ROOT}/thirdparty/glm/ext.hpp
    ${NOSTALGIA_ROOT}/rendering/graphics_context.hpp
    ${NOSTALGIA_ROOT}/rendering/renderer_api.hpp
    ${NOSTALGIA_ROOT}/rendering/common.hpp
    ${NOSTALGIA_ROOT}/rendering/debugging.hpp
    ${NOSTALGIA_ROOT}/rendering/vertex_array.hpp
    ${NOSTALGIA_ROOT}/rendering/environment.hpp
    ${NOSTALGIA_ROOT}/rendering/vertex.hpp
    ${NOSTALGIA_ROOT}/rendering/frame_buffer.hpp
    ${NOSTALGIA_ROOT}/rendering/mesh_buffers.hpp
    ${NOSTALGIA_ROOT}/rendering/shader.hpp
    ${NOSTALGIA_ROOT}/rendering/texture_buffer.hpp
    ${NOSTALGIA_ROOT}/console/console.hpp
    ${NOSTALGIA_ROOT}/common/colors.hpp
    ${NOSTALGIA_ROOT}/common/status_map.hpp
    ${NOSTALGIA_ROOT}/common/string_compare.hpp
    ${NOSTALGIA_ROOT}/components/game_loop.hpp
    ${NOSTALGIA_ROOT}/components/event_handling.hpp
    ${NOSTALGIA_ROOT}/fwd/ui.hpp
    ${NOSTALGIA_ROOT}/fwd/theatre.hpp
    ${NOSTALGIA_ROOT}/fwd/rendering.hpp
    ${NOSTALGIA_ROOT}/fwd/things.hpp
    ${NOSTALGIA_ROOT}/fwd/managers.hpp
    ${NOSTALGIA_ROOT}/fwd/core.hpp
    ${NOSTALGIA_ROOT}/fwd/events.hpp
    ${NOSTALGIA_ROOT}/fwd/event_types.hpp
    ${NOSTALGIA_ROOT}/fwd/application.hpp
    ${NOSTALGIA_ROOT}/Nostalgia.hpp
    ${NOSTALGIA_ROOT}/math/glm_format.hpp
    ${NOSTALGIA_ROOT}/math/conversion.hpp
    ${NOSTALGIA_ROOT}/math/glm_concepts.hpp)

if(UNIX)
    install(TARGETS Nostalgia DESTINATION lib)

    # Stolen from JoltPhysics/Build/CMakeLists.txt:338-345
    foreach(HEADER_FILE ${NOSTALGIA_HEADER_FILES})
        string(REPLACE ${NOSTALGIA_ROOT} "Nostalgia" RELATIVE_HEADER_FILE ${HEADER_FILE})
        get_filename_component(DESTINATION_PATH ${RELATIVE_HEADER_FILE} DIRECTORY)
        cmake_path(SET DST_FILE NORMALIZE "${CMAKE_INSTALL_INCLUDEDIR}/${DESTINATION_PATH}")
        install(FILES ${HEADER_FILE} DESTINATION ${DST_FILE})
    endforeach()
endif()
