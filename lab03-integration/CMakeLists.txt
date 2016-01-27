#================================
# Directory variables.
#================================
set(LAB03_ROOT "${ANIM_SOURCE_DIR}/lab03-integration")
set(LAB03_INCLUDE_ROOT "${LAB03_ROOT}/include")
set(LAB03_SOURCE_ROOT "${LAB03_ROOT}/source")
set(LAB03_SHADER_ROOT "${LAB03_ROOT}/shaders")
set(LAB03_GENERATED_ROOT "${LAB03_ROOT}/generated")

#================================
# Add subdirectories.
#================================
add_subdirectory(${LAB03_INCLUDE_ROOT})
add_subdirectory(${LAB03_SOURCE_ROOT})
add_subdirectory(${LAB03_SHADER_ROOT})

#================================
# Include directories.
#================================
include_directories(
    ${ATLAS_INCLUDE_DIRS}
    ${LAB03_INCLUDE_ROOT}
    ${LAB03_GENERATED_ROOT})

#================================
# Source groups.
#================================
source_group("include" FILES ${INCLUDE_LIST})
source_group("source" FILES ${SOURCE_LIST})
source_group("shaders" FILES ${SHADER_LIST})

add_executable(lab03-integration ${SOURCE_LIST} ${INCLUDE_LIST} ${SHADER_LIST})
target_link_libraries(lab03-integration ${ATLAS_LIBRARIES})

