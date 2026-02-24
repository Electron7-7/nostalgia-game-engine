cmake_minimum_required(VERSION 3.14...4.2)
project(libpng VERSION 1.8.0 LANGUAGES C)

message(STATUS "Configuring libpng")

include(CheckCSourceCompiles)
include(CheckLibraryExists)
include(GNUInstallDirs)

set(PNG_LIBCONF_HEADER_PREBUILT "${CMAKE_CURRENT_SOURCE_DIR}/pnglibconf.h.prebuilt")

# Allow the users to switch on/off the use of hardware (SIMD) optimized code.
set(PNG_HARDWARE_OPTIMIZATIONS ON)

# Initialize and show the target architecture variable PNG_TARGET_ARCHITECTURE.
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" PNG_TARGET_ARCHITECTURE)

# Find the math library (unless we already know it's not available or
# not needed).
if(UNIX AND NOT (APPLE OR BEOS OR HAIKU OR EMSCRIPTEN))
    check_library_exists(m pow "" PNG_HAVE_LIBM_POW)
endif()
if(PNG_HAVE_LIBM_POW)
    list(APPEND PNG_LINK_LIBRARIES m)
endif()

# Silence function deprecation warnings on the Windows compilers that might
# use the MSVC Runtime library headers.
if(WIN32 AND (CMAKE_C_COMPILER_ID MATCHES "MSVC|Intel|Clang"))
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

option(ld-version-script "Enable linker version script" ON)
if(ld-version-script AND NOT (ANDROID OR APPLE))
    # Check if LD supports linker scripts.
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/conftest.map" "
        VERS_1 { global: sym1; local: *; };
        VERS_2 { global: sym2; main; } VERS_1;
        ")
    set(_SAVED_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
    if(NOT CMAKE_HOST_SOLARIS)
        # Avoid using CMAKE_SHARED_LIBRARY_C_FLAGS in version script checks on
        # Solaris, because of an incompatibility with the Solaris link editor.
        list(APPEND CMAKE_REQUIRED_FLAGS ${CMAKE_SHARED_LIBRARY_C_FLAGS})
    endif()
    list(APPEND CMAKE_REQUIRED_FLAGS
      "-Wl,--version-script='${CMAKE_CURRENT_BINARY_DIR}/conftest.map'")
    check_c_source_compiles("
        void sym1(void) {}
        void sym2(void) {}
        int main(void) { return 0; }
        " HAVE_LD_VERSION_SCRIPT)
    if(NOT HAVE_LD_VERSION_SCRIPT)
        set(CMAKE_REQUIRED_FLAGS ${_SAVED_CMAKE_REQUIRED_FLAGS})
        if(NOT CMAKE_HOST_SOLARIS)
            # Again, avoid using CMAKE_SHARED_LIBRARY_C_FLAGS in version script
            # checks on Solaris.
            list(APPEND CMAKE_REQUIRED_FLAGS ${CMAKE_SHARED_LIBRARY_C_FLAGS})
        endif()
        list(APPEND CMAKE_REQUIRED_FLAGS "-Wl,-M -Wl,${CMAKE_CURRENT_BINARY_DIR}/conftest.map")
        check_c_source_compiles("
        void sym1(void) {}
        void sym2(void) {}
        int main(void) { return 0; }
        " HAVE_SOLARIS_LD_VERSION_SCRIPT)
    endif()
    set(CMAKE_REQUIRED_FLAGS ${_SAVED_CMAKE_REQUIRED_FLAGS})
    file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/conftest.map")
endif()

find_program(AWK NAMES gawk nawk awk)

# Include the internal module PNGCheckLibconf.cmake
include("${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/PNGCheckLibconf.cmake")

if(NOT AWK)
    message(STATUS "libpng: Could not find an AWK-compatible program")
    # No AWK program available to generate pnglibconf.h.
    # Configure libpng with pnglibconf.h.prebuilt.
    png_check_libconf(HEADER "${PNG_LIBCONF_HEADER_PREBUILT}")
    configure_file("${PNG_LIBCONF_HEADER_PREBUILT}"
        "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h"
        @ONLY)
    add_custom_target(png_genfiles)
else()
    png_check_libconf(DFA_XTRA "${DFA_XTRA}")

    # Include the internal module PNGGenConfig.cmake
    include("${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/PNGGenConfig.cmake")

    # Work around a limitation of various Windows AWK programs that are
    # unable to process CRLF-terminated AWK scripts.
    # Copy these AWK scripts to a temporary location, converting their
    # line endings from Windows (CRLF) to Unix (LF) at the destination.
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/checksym.awk"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/checksym.awk"
        @ONLY
        NEWLINE_STYLE LF)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/options.awk"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/options.awk"
        @ONLY
        NEWLINE_STYLE LF)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/dfn.awk"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/dfn.awk"
        @ONLY
        NEWLINE_STYLE LF)

    # Generate scripts/pnglibconf/pnglibconf.c
    generate_source(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/pnglibconf.c"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/options.awk"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/pnglibconf.dfa"
        "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h")

    # Generate pnglibconf.c
    generate_source(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.c"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/options.awk"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/pnglibconf.dfa"
        "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h")

    if(PNG_PREFIX)
        set(PNGLIBCONF_H_EXTRA_DEPENDS
            "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/prefix.out"
            "${CMAKE_CURRENT_SOURCE_DIR}/scripts/macro.lst")
        set(PNGPREFIX_H_EXTRA_DEPENDS
            "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/intprefix.out")
    endif()

    generate_out(INPUT "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.c"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.out")

    # Generate pnglibconf.h
    generate_source(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.out"
        ${PNGLIBCONF_H_EXTRA_DEPENDS})

    generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/intprefix.c"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/intprefix.out"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h")

    generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/prefix.c"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/prefix.out"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/png.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h"
        "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.out")

    # Generate pngprefix.h
    generate_source(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/pngprefix.h"
        DEPENDS ${PNGPREFIX_H_EXTRA_DEPENDS})

    generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/sym.c"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/sym.out"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h")

    generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/symbols.c"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/symbols.out"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/png.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/pnglibconf.h.prebuilt")

    generate_out(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/pnglibconf/vers.c"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/vers.out"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/png.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/pngconf.h"
        "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h")

    generate_chk(INPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/symbols.out"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/symbols.chk"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/checksym.awk"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/symbols.def")

    generate_copy(INPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/sym.out"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libpng.sym")

    generate_copy(INPUT "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/vers.out"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libpng.vers")

    add_custom_target(png_genprebuilt
        COMMAND "${CMAKE_COMMAND}"
        "-DOUTPUT=pnglibconf.h.prebuilt"
        -P "${CMAKE_CURRENT_BINARY_DIR}/scripts/cmake/gensrc.cmake"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    # A single target handles generation of all generated files.
    add_custom_target(png_genfiles
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/libpng.sym"
        "${CMAKE_CURRENT_BINARY_DIR}/libpng.vers"
        "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.c"
        "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h"
        "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.out"
        "${CMAKE_CURRENT_BINARY_DIR}/pngprefix.h"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/intprefix.out"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/pnglibconf.c"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/prefix.out"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/sym.out"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/symbols.chk"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/symbols.out"
        "${CMAKE_CURRENT_BINARY_DIR}/scripts/pnglibconf/vers.out")
endif()

# List the source code files.
set(libpng_public_hdrs
    png.h
    pngconf.h
    "${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h")
set(libpng_private_hdrs
    pngpriv.h
    pngdebug.h
    pnginfo.h
    pngstruct.h)

if(AWK)
    list(APPEND libpng_private_hdrs "${CMAKE_CURRENT_BINARY_DIR}/pngprefix.h")
endif()

set(libpng_sources
    ${libpng_public_hdrs}
    ${libpng_private_hdrs}
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
    pngsimd.c)

set(pngtest_sources
    pngtest.c)

set(pngvalid_sources
    contrib/libtests/pngvalid.c)

set(pngstest_sources
    contrib/libtests/pngstest.c)

set(pngunknown_sources
    contrib/libtests/pngunknown.c)

set(pngimage_sources
    contrib/libtests/pngimage.c)

set(pngfix_sources
    contrib/tools/pngfix.c)

set(png_fix_itxt_sources
    contrib/tools/png-fix-itxt.c)

# Create source generation scripts.
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/genchk.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/scripts/cmake/genchk.cmake"
   @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/genout.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/scripts/cmake/genout.cmake"
   @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/gensrc.cmake.in"
   "${CMAKE_CURRENT_BINARY_DIR}/scripts/cmake/gensrc.cmake"
   @ONLY)

add_library(png-object OBJECT ${libpng_sources})
add_dependencies(png-object png_genfiles)
target_include_directories(png-object
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")
target_link_libraries(png-object PUBLIC ${PNG_LINK_LIBRARIES})
