cmake_minimum_required(VERSION 3.12)
project(bzip2
    VERSION 1.1.0
    DESCRIPTION "This Bzip2/libbz2 a program and library for lossless block-sorting data compression."
    LANGUAGES C)

message(STATUS "Configuring bzip2")

set(LT_CURRENT  1)
set(LT_REVISION 9)
set(LT_AGE      0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(Version)
include(SymLink)

set(BZ_VERSION ${PROJECT_VERSION})
configure_file (
    ${PROJECT_SOURCE_DIR}/bz_version.h.in
    ${PROJECT_BINARY_DIR}/bz_version.h)
include_directories(${PROJECT_BINARY_DIR})

math(EXPR LT_SOVERSION "${LT_CURRENT} - ${LT_AGE}")
set(LT_VERSION "${LT_SOVERSION}.${LT_AGE}.${LT_REVISION}")
set(PACKAGE_VERSION ${PROJECT_VERSION})
HexVersion(PACKAGE_VERSION_NUM ${PROJECT_VERSION_MAJOR} ${PROJECT_VERSION_MINOR} ${PROJECT_VERSION_PATCH})

# Do not disable assertions based on CMAKE_BUILD_TYPE.
foreach(_build_type Release MinSizeRel RelWithDebInfo)
    foreach(_lang C)
        string(TOUPPER CMAKE_${_lang}_FLAGS_${_build_type} _var)
        string(REGEX REPLACE "(^|)[/-]D *NDEBUG($|)" " " ${_var} "${${_var}}")
    endforeach()
endforeach()

# Support the latest c++ standard available.
include(ExtractValidFlags)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Checks for header files.
include(CheckIncludeFile)
check_include_file(arpa/inet.h    HAVE_ARPA_INET_H)
check_include_file(fcntl.h        HAVE_FCNTL_H)
check_include_file(inttypes.h     HAVE_INTTYPES_H)
check_include_file(limits.h       HAVE_LIMITS_H)
check_include_file(netdb.h        HAVE_NETDB_H)
check_include_file(netinet/in.h   HAVE_NETINET_IN_H)
check_include_file(pwd.h          HAVE_PWD_H)
check_include_file(sys/socket.h   HAVE_SYS_SOCKET_H)
check_include_file(sys/time.h     HAVE_SYS_TIME_H)
check_include_file(syslog.h       HAVE_SYSLOG_H)
check_include_file(time.h         HAVE_TIME_H)
check_include_file(unistd.h       HAVE_UNISTD_H)

include(CheckTypeSize)
# Checks for typedefs, structures, and compiler characteristics.
# AC_TYPE_SIZE_T
check_type_size("ssize_t" SIZEOF_SSIZE_T)
if(NOT SIZEOF_SSIZE_T)
    # ssize_t is a signed type in POSIX storing at least -1.
    # Set it to "int" to match the behavior of AC_TYPE_SSIZE_T (autotools).
    set(ssize_t int)
endif()

include(CheckStructHasMember)
check_struct_has_member("struct tm" tm_gmtoff time.h HAVE_STRUCT_TM_TM_GMTOFF)

# Checks for library functions.
include(CheckFunctionExists)
check_function_exists(_Exit     HAVE__EXIT)
check_function_exists(accept4   HAVE_ACCEPT4)
check_function_exists(mkostemp  HAVE_MKOSTEMP)

include(CheckSymbolExists)
# XXX does this correctly detect initgroups (un)availability on cygwin?
check_symbol_exists(initgroups grp.h HAVE_DECL_INITGROUPS)
if(NOT HAVE_DECL_INITGROUPS AND HAVE_UNISTD_H)
    # FreeBSD declares initgroups() in unistd.h
    check_symbol_exists(initgroups unistd.h HAVE_DECL_INITGROUPS2)
    if(HAVE_DECL_INITGROUPS2)
        set(HAVE_DECL_INITGROUPS 1)
    endif()
endif()

# autotools-compatible names
# Sphinx expects relative paths in the .rst files. Use the fact that the files
# below are all one directory level deep.
file(RELATIVE_PATH top_srcdir   ${CMAKE_CURRENT_BINARY_DIR}/dir ${CMAKE_CURRENT_SOURCE_DIR})
file(RELATIVE_PATH top_builddir ${CMAKE_CURRENT_BINARY_DIR}/dir ${CMAKE_CURRENT_BINARY_DIR})
set(abs_top_srcdir   ${CMAKE_CURRENT_SOURCE_DIR})
set(abs_top_builddir ${CMAKE_CURRENT_BINARY_DIR})
# bzip2.pc (pkg-config file)
set(prefix      ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(bindir      ${CMAKE_INSTALL_FULL_BINDIR})
set(sbindir     ${CMAKE_INSTALL_FULL_SBINDIR})
set(libdir      ${CMAKE_INSTALL_FULL_LIBDIR})
set(includedir  ${CMAKE_INSTALL_FULL_INCLUDEDIR})
set(VERSION     ${PACKAGE_VERSION})

set(BZ2_SOURCES
    blocksort.c
    huffman.c
    crctable.c
    randtable.c
    compress.c
    decompress.c
    bzlib.c)

# The bz2 OBJECT-library, required for bzip2, bzip2recover.
add_library(bz2-object OBJECT)
target_sources(bz2-object
    PRIVATE   ${BZ2_SOURCES}
    PUBLIC    ${CMAKE_CURRENT_SOURCE_DIR}/bzlib_private.h
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/bzlib.h)
