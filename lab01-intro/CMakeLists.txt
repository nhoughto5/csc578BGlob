#================================
# Directory variables.
#================================
set(LAB01_ROOT "${ANIM_SOURCE_DIR}/lab01-intro")
set(LAB01_INCLUDE_ROOT "${LAB01_ROOT}/include")
set(LAB01_SOURCE_ROOT "${LAB01_ROOT}/source")
set(LAB01_SHADER_ROOT "${LAB01_ROOT}/shaders")
set(LAB01_GENERATED_ROOT "${LAB01_ROOT}/generated")

#================================
# Add subdirectories.
#================================
add_subdirectory(${LAB01_INCLUDE_ROOT})
add_subdirectory(${LAB01_SOURCE_ROOT})
add_subdirectory(${LAB01_SHADER_ROOT})

#================================
# Include directories.
#================================
include_directories(
    ${ATLAS_INCLUDE_DIRS}
    ${LAB01_INCLUDE_ROOT}
    ${LAB01_GENERATED_ROOT})

#================================
# Source groups.
#================================
source_group("include" FILES ${INCLUDE_LIST})
source_group("source" FILES ${SOURCE_LIST})
source_group("shaders" FILES ${SHADER_LIST})

add_executable(lab01-intro ${SOURCE_LIST} ${INCLUDE_LIST} ${SHADER_LIST})
target_link_libraries(lab01-intro ${ATLAS_LIBRARIES})

