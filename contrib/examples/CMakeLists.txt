# Example programs for libexif

# Find example source files
file(GLOB EXAMPLE_SOURCES "*.c")

# Create example executables
foreach(example_source ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_source} NAME_WE)
    add_executable(${example_name} ${example_source})
    target_link_libraries(${example_name} exif)
    
    # Install examples
    install(TARGETS ${example_name}
        RUNTIME DESTINATION bin
    )
endforeach()
