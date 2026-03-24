# Test programs for libexif

# Find test source files
file(GLOB TEST_SOURCES "*.c")
list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test-extract.c")

# Create test executables
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} exif)
    
    # Add test
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# Special handling for test-extract which needs testdata
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test-extract.c")
    add_executable(test-extract test-extract.c)
    target_link_libraries(test-extract exif)
    
    # Copy testdata to build directory
    add_custom_command(TARGET test-extract PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/testdata
        ${CMAKE_CURRENT_BINARY_DIR}/testdata
    )
    
    add_test(NAME test-extract COMMAND test-extract)
endif()

# Add custom test targets for shell scripts
if(UNIX)
    # Add shell script tests
    file(GLOB TEST_SCRIPTS "*.sh")
    foreach(script ${TEST_SCRIPTS})
        get_filename_component(script_name ${script} NAME_WE)
        add_test(NAME ${script_name} COMMAND ${script})
    endforeach()
endif()
