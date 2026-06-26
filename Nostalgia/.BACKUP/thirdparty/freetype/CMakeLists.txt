cmake_minimum_required(VERSION 3.12...3.31.0)
project(freetype VERSION 2.14.1 LANGUAGES C)

message(STATUS "Configuring freetype")

include(CheckIncludeFile)
include(CMakeDependentOption)

# Generate LIBRARY_VERSION and LIBRARY_SOVERSION.
set(LIBTOOL_REGEX "version_info='([0-9]+):([0-9]+):([0-9]+)'")
file(STRINGS "${PROJECT_SOURCE_DIR}/builds/unix/configure.raw"
    VERSION_INFO
    REGEX ${LIBTOOL_REGEX})
string(REGEX REPLACE
    ${LIBTOOL_REGEX} "\\1"
    LIBTOOL_CURRENT "${VERSION_INFO}")
string(REGEX REPLACE
    ${LIBTOOL_REGEX} "\\2"
    LIBTOOL_REVISION "${VERSION_INFO}")
string(REGEX REPLACE
    ${LIBTOOL_REGEX} "\\3"
    LIBTOOL_AGE "${VERSION_INFO}")

# This is what libtool does internally on Unix platforms.
math(EXPR LIBRARY_SOVERSION "${LIBTOOL_CURRENT} - ${LIBTOOL_AGE}")
set(LIBRARY_VERSION "${LIBRARY_SOVERSION}.${LIBTOOL_AGE}.${LIBTOOL_REVISION}")

# Create the configuration file
if(UNIX AND NOT WIN32)
    check_include_file("unistd.h" HAVE_UNISTD_H)
    check_include_file("fcntl.h" HAVE_FCNTL_H)

    file(READ "${PROJECT_SOURCE_DIR}/builds/unix/ftconfig.h.in"
    FTCONFIG_H)
    if(HAVE_UNISTD_H)
        string(REGEX REPLACE
            "#undef +(HAVE_UNISTD_H)" "#define \\1 1"
            FTCONFIG_H "${FTCONFIG_H}")
    endif()
    if(HAVE_FCNTL_H)
        string(REGEX REPLACE
            "#undef +(HAVE_FCNTL_H)" "#define \\1 1"
            FTCONFIG_H "${FTCONFIG_H}")
    endif()
else()
    file(READ "${PROJECT_SOURCE_DIR}/include/freetype/config/ftconfig.h"
        FTCONFIG_H)
endif()

set(FTCONFIG_H_NAME "${PROJECT_BINARY_DIR}/include/freetype/config/ftconfig.h")
if(EXISTS "${FTCONFIG_H_NAME}")
    file(READ "${FTCONFIG_H_NAME}" ORIGINAL_FTCONFIG_H)
else()
    set(ORIGINAL_FTCONFIG_H "")
endif()
if(NOT (ORIGINAL_FTCONFIG_H STREQUAL FTCONFIG_H))
    file(WRITE "${FTCONFIG_H_NAME}" "${FTCONFIG_H}")
endif()

# Create the options file
file(READ "${PROJECT_SOURCE_DIR}/include/freetype/config/ftoption.h"
    FTOPTION_H)

set(FTOPTION_H_NAME "${PROJECT_BINARY_DIR}/include/freetype/config/ftoption.h")
if(EXISTS "${FTOPTION_H_NAME}")
    file(READ "${FTOPTION_H_NAME}" ORIGINAL_FTOPTION_H)
else()
    set(ORIGINAL_FTOPTION_H "")
endif()
if(NOT (ORIGINAL_FTOPTION_H STREQUAL FTOPTION_H))
    file(WRITE "${FTOPTION_H_NAME}" "${FTOPTION_H}")
endif()

file(GLOB PUBLIC_HEADERS "include/ft2build.h" "include/freetype/*.h")
file(GLOB PUBLIC_CONFIG_HEADERS "include/freetype/config/*.h")
file(GLOB PRIVATE_HEADERS "include/freetype/internal/*.h")

set(BASE_SRCS
    src/autofit/autofit.c
    src/base/ftbase.c
    src/base/ftbbox.c
    src/base/ftbdf.c
    src/base/ftbitmap.c
    src/base/ftcid.c
    src/base/ftfstype.c
    src/base/ftgasp.c
    src/base/ftglyph.c
    src/base/ftgxval.c
    src/base/ftinit.c
    src/base/ftmm.c
    src/base/ftotval.c
    src/base/ftpatent.c
    src/base/ftpfr.c
    src/base/ftstroke.c
    src/base/ftsynth.c
    src/base/fttype1.c
    src/base/ftwinfnt.c
    src/bdf/bdf.c
    src/bzip2/ftbzip2.c
    src/cache/ftcache.c
    src/cff/cff.c
    src/cid/type1cid.c
    src/gzip/ftgzip.c
    src/lzw/ftlzw.c
    src/pcf/pcf.c
    src/pfr/pfr.c
    src/psaux/psaux.c
    src/pshinter/pshinter.c
    src/psnames/psnames.c
    src/raster/raster.c
    src/sdf/sdf.c
    src/sfnt/sfnt.c
    src/smooth/smooth.c
    src/svg/svg.c
    src/truetype/truetype.c
    src/type1/type1.c
    src/type42/type42.c
    src/winfonts/winfnt.c)

if(WIN32)
    list(APPEND BASE_SRCS "builds/windows/ftsystem.c")
elseif(UNIX)
    list(APPEND BASE_SRCS "builds/unix/ftsystem.c")
else()
    list(APPEND BASE_SRCS "src/base/ftsystem.c")
endif()

if(WIN32)
    enable_language(RC)
    list(APPEND BASE_SRCS builds/windows/ftdebug.c
        src/base/ftver.rc)
elseif(WINCE)
    list(APPEND BASE_SRCS builds/wince/ftdebug.c)
else()
    list(APPEND BASE_SRCS src/base/ftdebug.c)
endif()

add_library(freetype-object OBJECT
    ${PUBLIC_HEADERS}
    ${PUBLIC_CONFIG_HEADERS}
    ${PRIVATE_HEADERS}
    ${BASE_SRCS})

target_compile_definitions(freetype-object PRIVATE  FT2_BUILD_LIBRARY)

if(WIN32)
    target_compile_definitions(freetype-object PRIVATE
        _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
else()
    # Windows handles exports with DLL_EXPORT defined above.
    set_target_properties(freetype-object PROPERTIES
        C_VISIBILITY_PRESET hidden)
endif()

target_include_directories(
    freetype-object
    PUBLIC
    $<INSTALL_INTERFACE:include/freetype2>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../brotli/c/include

    # Make <ftconfig.h> available for builds/unix/ftsystem.c.
    ${CMAKE_CURRENT_BINARY_DIR}/include/freetype/config)
