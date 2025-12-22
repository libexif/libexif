# Example programs for libexif

# Option to enable cam_features example (Linux only)
if(UNIX AND NOT APPLE)
    option(BUILD_CAM_FEATURES "Build cam_features example (requires Linux-specific libraries)" OFF)
endif()

# List of example sources excluding cam_features.c
set(EXAMPLE_SOURCES
    photographer.c
    thumbnail.c
    write-exif.c
)

# Add cam_features if enabled and on Linux
if(BUILD_CAM_FEATURES)
    if(UNIX AND NOT APPLE)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(RAW1394 REQUIRED libraw1394)
        pkg_check_modules(DC1394 REQUIRED libdc1394-2)
        
        list(APPEND EXAMPLE_SOURCES cam_features.c)
        
        message(STATUS "cam_features example will be built")
    else()
        message(WARNING "cam_features can only be built on Linux - ignoring BUILD_CAM_FEATURES")
    endif()
endif()

# Create example executables
foreach(example_source ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_source} NAME_WE)
    add_executable(${example_name} ${example_source})
    
    # Link with all required libexif components
    target_link_libraries(${example_name} PRIVATE
        exif
        mnote-canon
        mnote-fuji
        mnote-olympus
        mnote-pentax
    )
    
    # Special handling for cam_features
    if(example_name STREQUAL "cam_features")
        target_include_directories(${example_name} PRIVATE
            ${RAW1394_INCLUDE_DIRS}
            ${DC1394_INCLUDE_DIRS}
        )
        target_link_libraries(${example_name} PRIVATE
            ${RAW1394_LIBRARIES}
            ${DC1394_LIBRARIES}
        )
    endif()
    
    # Install examples
    install(TARGETS ${example_name}
        RUNTIME DESTINATION bin
    )
endforeach()