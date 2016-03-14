#================================
# Directory variables.
#================================
set(GLOB_ROOT "${ANIM_SOURCE_DIR}/Glob")
set(GLOB_INCLUDE_ROOT "${GLOB_ROOT}/include")
set(GLOB_SOURCE_ROOT "${GLOB_ROOT}/source")
set(GLOB_SHADER_ROOT "${GLOB_ROOT}/shaders")
set(GLOB_GENERATED_ROOT "${GLOB_ROOT}/generated")

#================================
# Add subdirectories.
#================================
add_subdirectory(${GLOB_INCLUDE_ROOT})
add_subdirectory(${GLOB_SOURCE_ROOT})
add_subdirectory(${GLOB_SHADER_ROOT})

#================================
# Include directories.
#================================
include_directories(
    ${ATLAS_INCLUDE_DIRS}
    ${GLOB_INCLUDE_ROOT}
    ${GLOB_GENERATED_ROOT})

#================================
# Source groups.
#================================
source_group("include" FILES ${INCLUDE_LIST})
source_group("source" FILES ${SOURCE_LIST})
source_group("shaders" FILES ${SHADER_LIST})

add_executable(Glob ${SOURCE_LIST} ${INCLUDE_LIST} ${SHADER_LIST})
target_link_libraries(Glob ${ATLAS_LIBRARIES})