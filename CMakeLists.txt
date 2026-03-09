cmake_minimum_required(VERSION 3.10)

# Project information
project(libexif VERSION 0.6.25.1 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard (as requested)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set MSVC runtime library (allow override from command line)
if(NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThread$<$<CONFIG:Debug>:Debug>")
endif()

# Ensure static runtime for MSVC
if(MSVC)
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()

# Build static library by default
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Debug postfix (allow override from command line)
if(NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Set policy version minimum
cmake_policy(VERSION 3.10)

# Options
option(ENABLE_NLS "Enable native language support" OFF)
option(ENABLE_INTERNAL_DOCS "Enable internal documentation" OFF)
option(ENABLE_SHIP_BINARIES "Enable shipping binaries" OFF)

# Example options
option(BUILD_EXAMPLES "Build example programs" OFF)
if(BUILD_EXAMPLES AND UNIX AND NOT APPLE)
    option(BUILD_CAM_FEATURES "Build cam_features example (requires Linux-specific libraries)" OFF)
endif()

# Find required packages
if(ENABLE_NLS)
    find_package(Intl REQUIRED)
    set(HAVE_GETTEXT 1)
    set(ENABLE_NLS 1)
endif()

# Check for required features
include(CheckIncludeFile)
include(CheckFunctionExists)
include(TestBigEndian)

# Platform detection
if(MSVC)
    # Always copy _stdint.h for MSVC as a fallback
    configure_file(
        ${CMAKE_SOURCE_DIR}/contrib/watcom/_stdint.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libexif/_stdint.h
        COPYONLY
    )
    
    # Check if system stdint.h exists
    check_include_file(stdint.h HAVE_STDINT_H)
    
    set(HAVE_LOCALTIME_S 1)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
    )
else()
    check_include_file(stdint.h HAVE_STDINT_H)
    check_function_exists(localtime_s HAVE_LOCALTIME_S)
    check_function_exists(localtime_r HAVE_LOCALTIME_R)
endif()

# Ensure all necessary include paths are available
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
)

# Check endianness
TEST_BIG_ENDIAN(WORDS_BIGENDIAN)

# Check for gettext
if(GETTEXT_FOUND)
    set(HAVE_GETTEXT 1)
    set(ENABLE_NLS 1)
endif()

# Generate config.h
configure_file(config.h.in config.h)



# Add common definitions
add_compile_definitions(
    PACKAGE_NAME="${PROJECT_NAME}"
    PACKAGE_VERSION="${PROJECT_VERSION}"
    PACKAGE_STRING="${PROJECT_NAME} ${PROJECT_VERSION}"
    PACKAGE_BUGREPORT="libexif-devel@lists.sourceforge.net"
    PACKAGE_URL="https://libexif.github.io/"
    GETTEXT_PACKAGE="${PROJECT_NAME}"
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler flags and definitions
if(MSVC)
    add_compile_options(/W3)
    add_compile_definitions(
        "inline=__inline"
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
    )
else()
    add_compile_options(-Wall -Wextra -Wmissing-declarations -Wmissing-prototypes)
endif()


# Version information
set(LIBEXIF_CURRENT 15)
set(LIBEXIF_AGE 3)
set(LIBEXIF_REVISION 4)
set(LIBEXIF_VERSION_INFO "${LIBEXIF_CURRENT}:${LIBEXIF_REVISION}:${LIBEXIF_AGE}")

# Canon makernote sources
set(CANON_SOURCES
    libexif/canon/exif-mnote-data-canon.c
    libexif/canon/mnote-canon-entry.c
    libexif/canon/mnote-canon-tag.c
)

set(CANON_HEADERS
    libexif/canon/exif-mnote-data-canon.h
    libexif/canon/mnote-canon-entry.h
    libexif/canon/mnote-canon-tag.h
)

# Fuji makernote sources
set(FUJI_SOURCES
    libexif/fuji/exif-mnote-data-fuji.c
    libexif/fuji/mnote-fuji-entry.c
    libexif/fuji/mnote-fuji-tag.c
)

set(FUJI_HEADERS
    libexif/fuji/exif-mnote-data-fuji.h
    libexif/fuji/mnote-fuji-entry.h
    libexif/fuji/mnote-fuji-tag.h
)

# Olympus makernote sources
set(OLYMPUS_SOURCES
    libexif/olympus/exif-mnote-data-olympus.c
    libexif/olympus/mnote-olympus-entry.c
    libexif/olympus/mnote-olympus-tag.c
)

set(OLYMPUS_HEADERS
    libexif/olympus/exif-mnote-data-olympus.h
    libexif/olympus/mnote-olympus-entry.h
    libexif/olympus/mnote-olympus-tag.h
)

# Pentax makernote sources
set(PENTAX_SOURCES
    libexif/pentax/exif-mnote-data-pentax.c
    libexif/pentax/mnote-pentax-entry.c
    libexif/pentax/mnote-pentax-tag.c
)

set(PENTAX_HEADERS
    libexif/pentax/exif-mnote-data-pentax.h
    libexif/pentax/mnote-pentax-entry.h
    libexif/pentax/mnote-pentax-tag.h
)

# Main libexif sources
set(LIBEXIF_SOURCES
    libexif/exif-byte-order.c
    libexif/exif-content.c
    libexif/exif-data.c
    libexif/exif-entry.c
    libexif/exif-format.c
    libexif/exif-ifd.c
    libexif/exif-loader.c
    libexif/exif-log.c
    libexif/exif-mem.c
    libexif/exif-mnote-data.c
    libexif/exif-tag.c
    libexif/exif-utils.c
    libexif/exif-gps-ifd.c
)

# Main libexif headers
set(LIBEXIF_HEADERS
    libexif/exif-byte-order.h
    libexif/exif-content.h
    libexif/exif-data.h
    libexif/exif-data-type.h
    libexif/exif-entry.h
    libexif/exif-format.h
    libexif/exif-ifd.h
    libexif/exif-loader.h
    libexif/exif-log.h
    libexif/exif-mem.h
    libexif/exif-mnote-data.h
    libexif/exif-tag.h
    libexif/exif-utils.h
    libexif/exif-gps-ifd.h
    libexif/exif-system.h
    libexif/exif.h
    libexif/i18n.h
    libexif/exif-mnote-data-priv.h
)

# Create makernote libraries
add_library(mnote-canon STATIC ${CANON_SOURCES})
target_include_directories(mnote-canon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(mnote-canon PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
if(GETTEXT_FOUND)
    target_link_libraries(mnote-canon ${GETTEXT_LIBRARIES})
endif()

add_library(mnote-fuji STATIC ${FUJI_SOURCES})
target_include_directories(mnote-fuji PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(mnote-fuji PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
if(GETTEXT_FOUND)
    target_link_libraries(mnote-fuji ${GETTEXT_LIBRARIES})
endif()

add_library(mnote-olympus STATIC ${OLYMPUS_SOURCES})
target_include_directories(mnote-olympus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(mnote-olympus PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
if(GETTEXT_FOUND)
    target_link_libraries(mnote-olympus ${GETTEXT_LIBRARIES})
endif()

add_library(mnote-pentax STATIC ${PENTAX_SOURCES})
target_include_directories(mnote-pentax PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(mnote-pentax PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
if(GETTEXT_FOUND)
    target_link_libraries(mnote-pentax ${GETTEXT_LIBRARIES})
endif()

# Create main libexif library
if(BUILD_SHARED_LIBS)
    add_library(exif SHARED ${LIBEXIF_SOURCES})
else()
    add_library(exif STATIC ${LIBEXIF_SOURCES})
endif()

# Set library properties
set_target_properties(exif PROPERTIES
    VERSION ${LIBEXIF_CURRENT}.${LIBEXIF_REVISION}.${LIBEXIF_AGE}
    SOVERSION ${LIBEXIF_CURRENT}
    OUTPUT_NAME exif
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
)

# Link dependencies
target_link_libraries(exif
    mnote-canon
    mnote-fuji
    mnote-olympus
    mnote-pentax
)

if(GETTEXT_FOUND)
    target_link_libraries(exif ${GETTEXT_LIBRARIES})
endif()

# Include directories for main library
target_include_directories(exif PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Install targets
install(TARGETS exif mnote-canon mnote-fuji mnote-olympus mnote-pentax
    EXPORT libexifTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install headers
install(FILES ${LIBEXIF_HEADERS}
    DESTINATION include/libexif
)

# Install makernote headers
install(FILES ${CANON_HEADERS}
    DESTINATION include/libexif/canon
)

install(FILES ${FUJI_HEADERS}
    DESTINATION include/libexif/fuji
)

install(FILES ${OLYMPUS_HEADERS}
    DESTINATION include/libexif/olympus
)

install(FILES ${PENTAX_HEADERS}
    DESTINATION include/libexif/pentax
)

# Create and install pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libexif.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libexif.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libexif.pc
    DESTINATION lib/pkgconfig
)

# Export targets
install(EXPORT libexifTargets
    FILE libexifTargets.cmake
    NAMESPACE libexif::
    DESTINATION lib/cmake/libexif
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    libexifConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libexifConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/libexifConfig.cmake
    INSTALL_DESTINATION lib/cmake/libexif
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libexifConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libexifConfigVersion.cmake
    DESTINATION lib/cmake/libexif
)

# Optional: Build tests
option(BUILD_TESTS "Build test programs" OFF)
if(BUILD_TESTS)
    add_subdirectory(test)
endif()

# Optional: Build examples
option(BUILD_EXAMPLES "Build example programs" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(contrib/examples)
endif()

# Optional: Build documentation
option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(doc)
    endif()
endif()

# After check_include_file(stdint.h HAVE_STDINT_H)
if(NOT HAVE_STDINT_H)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libexif)
    configure_file(
        ${CMAKE_SOURCE_DIR}/contrib/watcom/_stdint.h
        ${CMAKE_BINARY_DIR}/libexif/_stdint.h
        COPYONLY
    )
endif()

# Ensure the build directory's libexif/ is in the include path
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/libexif
    ${CMAKE_CURRENT_SOURCE_DIR}
)
