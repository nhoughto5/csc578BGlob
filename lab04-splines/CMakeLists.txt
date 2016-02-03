#================================
# Directory variables.
#================================
set(LAB04_ROOT "${ANIM_SOURCE_DIR}/lab04-splines")
set(LAB04_INCLUDE_ROOT "${LAB04_ROOT}/include")
set(LAB04_SOURCE_ROOT "${LAB04_ROOT}/source")
set(LAB04_SHADER_ROOT "${LAB04_ROOT}/shaders")
set(LAB04_BINARY_ROOT "${CMAKE_CURRENT_BINARY_DIR}")

#================================
# Add subdirectories.
#================================
add_subdirectory(${LAB04_INCLUDE_ROOT})
add_subdirectory(${LAB04_SOURCE_ROOT})
add_subdirectory(${LAB04_SHADER_ROOT})

#================================
# Include directories.
#================================
include_directories(
    ${ATLAS_INCLUDE_DIRS}
    ${LAB04_INCLUDE_ROOT}
    ${LAB04_GENERATED_ROOT})

#================================
# Source groups.
#================================
source_group("include" FILES ${INCLUDE_LIST})
source_group("source" FILES ${SOURCE_LIST})
source_group("shaders" FILES ${SHADER_LIST})

add_executable(lab04-splines ${SOURCE_LIST} ${INCLUDE_LIST} ${SHADER_LIST})
target_link_libraries(lab04-splines ${ATLAS_LIBRARIES})

add_custom_command(
    TARGET lab04-splines POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${LAB04_SHADER_ROOT} ${LAB04_BINARY_ROOT}/shaders)
