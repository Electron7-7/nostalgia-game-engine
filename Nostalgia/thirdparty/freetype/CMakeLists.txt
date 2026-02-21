cmake_minimum_required(VERSION 3.12...3.31.0)

include(CheckIncludeFile)
include(CMakeDependentOption)

project(freetype C)

set(VERSION_MAJOR "2")
set(VERSION_MINOR "14")
set(VERSION_PATCH "1")

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

# External dependency library detection is automatic.  See the notes at the
# top of this file, for how to force or disable dependencies completely.
option(FT_DISABLE_ZLIB
  "Disable use of system zlib and use internal zlib library instead." OFF)
cmake_dependent_option(FT_REQUIRE_ZLIB
  "Require system zlib instead of internal zlib library." OFF
  "NOT FT_DISABLE_ZLIB" OFF)

option(FT_DISABLE_BZIP2
  "Disable support of bzip2 compressed fonts." OFF)
cmake_dependent_option(FT_REQUIRE_BZIP2
  "Require support of bzip2 compressed fonts." OFF
  "NOT FT_DISABLE_BZIP2" OFF)

option(FT_DISABLE_PNG
  "Disable support of PNG compressed OpenType embedded bitmaps." OFF)
cmake_dependent_option(FT_REQUIRE_PNG
  "Require support of PNG compressed OpenType embedded bitmaps." OFF
  "NOT FT_DISABLE_PNG" OFF)

option(FT_DISABLE_HARFBUZZ
  "Disable HarfBuzz (used for improving auto-hinting of OpenType fonts)." OFF)
cmake_dependent_option(FT_DYNAMIC_HARFBUZZ
  "Load HarfBuzz library dynamically at runtime, if possible." ON
  "NOT FT_DISABLE_HARFBUZZ" OFF)
cmake_dependent_option(FT_REQUIRE_HARFBUZZ
  "Require HarfBuzz for improving auto-hinting of OpenType fonts." OFF
  "NOT FT_DISABLE_HARFBUZZ" OFF)

option(FT_DISABLE_BROTLI
  "Disable support of compressed WOFF2 fonts." OFF)
cmake_dependent_option(FT_REQUIRE_BROTLI
  "Require support of compressed WOFF2 fonts." OFF
  "NOT FT_DISABLE_BROTLI" OFF)

option(FT_ENABLE_ERROR_STRINGS
  "Enable support for meaningful error descriptions." OFF)

# Disallow in-source builds
if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR
    "In-source builds are not permitted!  Make a separate folder for"
    " building, e.g.,\n"
    "  cmake -E make_directory build\n"
    "  cmake -E chdir build cmake ..\n"
    "Before that, remove the files created by this failed run with\n"
    "  cmake -E remove CMakeCache.txt\n"
    "  cmake -E remove_directory CMakeFiles")
endif ()


# Add local cmake modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/builds/cmake)

if (BUILD_FRAMEWORK)
  if (NOT "${CMAKE_GENERATOR}" STREQUAL "Xcode")
    message(FATAL_ERROR
      "You should use Xcode generator with BUILD_FRAMEWORK enabled")
  endif ()
  set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")
  set(BUILD_SHARED_LIBS ON)
endif ()

# Find dependencies
include(FindPkgConfig)
include(CMakePushCheckState)
include(CheckSymbolExists)

if (NOT FT_DISABLE_HARFBUZZ)
  set(HARFBUZZ_MIN_VERSION "2.0.0")

  if (FT_DYNAMIC_HARFBUZZ)
    if (WIN32)  # Windows uses its own LoadLibrary()
      set(FT_DYNAMIC_HARFBUZZ_ENABLED TRUE)
    else()
      cmake_push_check_state(RESET)
      check_symbol_exists(dlopen "dlfcn.h" HAVE_DLOPEN_IN_LIBC)
      if (NOT HAVE_DLOPEN_IN_LIBC)
        set(CMAKE_REQUIRED_LIBRARIES dl)
        check_symbol_exists(dlopen "dlfcn.h" HAVE_DLOPEN_IN_LIBDL)
      endif()
      cmake_pop_check_state()

      if (HAVE_DLOPEN_IN_LIBC OR HAVE_DLOPEN_IN_LIBDL)
        set(FT_DYNAMIC_HARFBUZZ_ENABLED TRUE)
        if (HAVE_DLOPEN_IN_LIBDL)
          set(FT_NEED_LIBDL TRUE)
        endif()
      endif()
    endif()
  endif()

  if (FT_DYNAMIC_HARFBUZZ_ENABLED)
    message(STATUS "Enabled dynamic loading of HarfBuzz library at runtime.")
  elseif (FT_REQUIRE_HARFBUZZ)
    find_package(HarfBuzz ${HARFBUZZ_MIN_VERSION} REQUIRED)
  else ()
    find_package(HarfBuzz ${HARFBUZZ_MIN_VERSION})
  endif ()
endif ()

if (NOT FT_DISABLE_PNG)
  if (FT_REQUIRE_PNG)
    find_package(PNG REQUIRED)
  else ()
    find_package(PNG)
  endif ()
  # FreePNG calls FindZLIB so unset ZLIB_FOUND to respect FT_DISABLE_ZLIB
  unset(ZLIB_FOUND)
endif ()

if (NOT FT_DISABLE_ZLIB)
  if (FT_REQUIRE_ZLIB)
    find_package(ZLIB REQUIRED)
  else ()
    find_package(ZLIB)
  endif ()
endif ()

if (NOT FT_DISABLE_BZIP2)
  # Genuine BZip2 does not provide bzip2.pc, but some platforms have it.
  # For better dependency in freetype2.pc, bzip2.pc is searched
  # regardless of the availability of libbz2. If bzip2.pc is found,
  # Requires.private is used instead of Libs.private.
  if (FT_REQUIRE_BZIP2)
    find_package(BZip2 REQUIRED)
  else ()
    find_package(BZip2)
  endif ()
  pkg_check_modules(PC_BZIP2 bzip2)
endif ()

if (NOT FT_DISABLE_BROTLI)
  if (FT_REQUIRE_BROTLI)
    find_package(BrotliDec REQUIRED)
  else ()
    find_package(BrotliDec)
  endif ()
endif ()

# Create the configuration file
if (UNIX AND NOT WIN32)
  check_include_file("unistd.h" HAVE_UNISTD_H)
  check_include_file("fcntl.h" HAVE_FCNTL_H)

  file(READ "${PROJECT_SOURCE_DIR}/builds/unix/ftconfig.h.in"
    FTCONFIG_H)
  if (HAVE_UNISTD_H)
    string(REGEX REPLACE
      "#undef +(HAVE_UNISTD_H)" "#define \\1 1"
      FTCONFIG_H "${FTCONFIG_H}")
  endif ()
  if (HAVE_FCNTL_H)
    string(REGEX REPLACE
      "#undef +(HAVE_FCNTL_H)" "#define \\1 1"
      FTCONFIG_H "${FTCONFIG_H}")
  endif ()
else ()
  file(READ "${PROJECT_SOURCE_DIR}/include/freetype/config/ftconfig.h"
    FTCONFIG_H)
endif ()

set(FTCONFIG_H_NAME "${PROJECT_BINARY_DIR}/include/freetype/config/ftconfig.h")
if (EXISTS "${FTCONFIG_H_NAME}")
  file(READ "${FTCONFIG_H_NAME}" ORIGINAL_FTCONFIG_H)
else ()
  set(ORIGINAL_FTCONFIG_H "")
endif ()
if (NOT (ORIGINAL_FTCONFIG_H STREQUAL FTCONFIG_H))
  file(WRITE "${FTCONFIG_H_NAME}" "${FTCONFIG_H}")
endif ()


# Create the options file
file(READ "${PROJECT_SOURCE_DIR}/include/freetype/config/ftoption.h"
  FTOPTION_H)
if (ZLIB_FOUND)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_SYSTEM_ZLIB) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()
if (BZIP2_FOUND)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_USE_BZIP2) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()
if (PNG_FOUND)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_USE_PNG) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()
if (HARFBUZZ_FOUND OR FT_DYNAMIC_HARFBUZZ_ENABLED)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_USE_HARFBUZZ) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
  if (FT_DYNAMIC_HARFBUZZ_ENABLED)
    string(REGEX REPLACE
      "/\\* +(#define +FT_CONFIG_OPTION_USE_HARFBUZZ_DYNAMIC) +\\*/" "\\1"
      FTOPTION_H "${FTOPTION_H}")
  endif ()
endif ()
if (BROTLIDEC_FOUND)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_USE_BROTLI) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()

if (FT_ENABLE_ERROR_STRINGS)
  string(REGEX REPLACE
    "/\\* +(#define +FT_CONFIG_OPTION_ERROR_STRINGS) +\\*/" "\\1"
    FTOPTION_H "${FTOPTION_H}")
endif ()

set(FTOPTION_H_NAME "${PROJECT_BINARY_DIR}/include/freetype/config/ftoption.h")
if (EXISTS "${FTOPTION_H_NAME}")
  file(READ "${FTOPTION_H_NAME}" ORIGINAL_FTOPTION_H)
else ()
  set(ORIGINAL_FTOPTION_H "")
endif ()
if (NOT (ORIGINAL_FTOPTION_H STREQUAL FTOPTION_H))
  file(WRITE "${FTOPTION_H_NAME}" "${FTOPTION_H}")
endif ()

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
  src/winfonts/winfnt.c
)

if (WIN32)
  list(APPEND BASE_SRCS "builds/windows/ftsystem.c")
elseif (UNIX)
  list(APPEND BASE_SRCS "builds/unix/ftsystem.c")
else ()
  list(APPEND BASE_SRCS "src/base/ftsystem.c")
endif ()

if (WIN32)
  enable_language(RC)
  list(APPEND BASE_SRCS builds/windows/ftdebug.c
                        src/base/ftver.rc)
elseif (WINCE)
  list(APPEND BASE_SRCS builds/wince/ftdebug.c)
else ()
  list(APPEND BASE_SRCS src/base/ftdebug.c)
endif ()

if (BUILD_FRAMEWORK)
  list(APPEND BASE_SRCS builds/mac/freetype-Info.plist)
endif ()


if (NOT DISABLE_FORCE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif ()

add_library(freetype_object OBJECT
  ${PUBLIC_HEADERS}
  ${PUBLIC_CONFIG_HEADERS}
  ${PRIVATE_HEADERS}
  ${BASE_SRCS}
)

target_compile_definitions(
  freetype_object PRIVATE FT2_BUILD_LIBRARY)

if (WIN32)
  target_compile_definitions(
    freetype_object PRIVATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)
else ()
  # Windows handles exports with DLL_EXPORT defined above.
  set_target_properties(freetype_object PROPERTIES
    C_VISIBILITY_PRESET hidden)
endif ()

target_include_directories(
  freetype_object
    PUBLIC
      $<INSTALL_INTERFACE:include/freetype2>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include

      # Make <ftconfig.h> available for builds/unix/ftsystem.c.
      ${CMAKE_CURRENT_BINARY_DIR}/include/freetype/config
)

set(PKGCONFIG_REQUIRES "")
set(PKGCONFIG_REQUIRES_PRIVATE "")
set(PKGCONFIG_LIBS "-L\${libdir} -lfreetype")
set(PKGCONFIG_LIBS_PRIVATE "")

if (ZLIB_FOUND)
  target_link_libraries(freetype_object PRIVATE ZLIB::ZLIB)
  list(APPEND PKGCONFIG_REQUIRES_PRIVATE "zlib")
endif ()
if (BZIP2_FOUND)
  target_link_libraries(freetype_object PRIVATE BZip2::BZip2)
  if (PC_BZIP2_FOUND)
    list(APPEND PKGCONFIG_REQUIRES_PRIVATE "bzip2")
  else ()
    list(APPEND PKGCONFIG_LIBS_PRIVATE "-lbz2")
  endif ()
endif ()
if (PNG_FOUND)
  target_link_libraries(freetype_object PRIVATE PNG::PNG)
  list(APPEND PKGCONFIG_REQUIRES_PRIVATE "libpng")
endif ()
if (FT_DYNAMIC_HARFBUZZ_ENABLED AND FT_NEED_LIBDL)
  target_link_libraries(freetype_object PRIVATE dl)
  list(APPEND PKGCONFIG_LIBS_PRIVATE " -ldl")
endif ()
if (HarfBuzz_FOUND AND (NOT FT_DYNAMIC_HARFBUZZ_ENABLED))
  target_link_libraries(freetype_object PRIVATE HarfBuzz::HarfBuzz)
  list(APPEND PKGCONFIG_REQUIRES_PRIVATE "harfbuzz >= ${HARFBUZZ_MIN_VERSION}")
endif ()
if (BROTLIDEC_FOUND)
  target_link_libraries(freetype_object PRIVATE ${BROTLIDEC_LIBRARIES})
  target_compile_definitions(freetype_object PRIVATE ${BROTLIDEC_DEFINITIONS})
  target_include_directories(freetype_object PRIVATE ${BROTLIDEC_INCLUDE_DIRS})
  list(APPEND PKGCONFIG_REQUIRES_PRIVATE "libbrotlidec")
endif ()
