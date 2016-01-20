#================================
# Directory variables.
#================================
set(LAB02_ROOT "${ANIM_SOURCE_DIR}/lab02-euler")
set(LAB02_INCLUDE_ROOT "${LAB02_ROOT}/include")
set(LAB02_SOURCE_ROOT "${LAB02_ROOT}/source")
set(LAB02_SHADER_ROOT "${LAB02_ROOT}/shaders")
set(LAB02_GENERATED_ROOT "${LAB02_ROOT}/generated")

#================================
# Add subdirectories.
#================================
add_subdirectory(${LAB02_INCLUDE_ROOT})
add_subdirectory(${LAB02_SOURCE_ROOT})
add_subdirectory(${LAB02_SHADER_ROOT})

#================================
# Include directories.
#================================
include_directories(
    ${ATLAS_INCLUDE_DIRS}
    ${LAB02_INCLUDE_ROOT}
    ${LAB02_GENERATED_ROOT})

#================================
# Source groups.
#================================
source_group("include" FILES ${INCLUDE_LIST})
source_group("source" FILES ${SOURCE_LIST})
source_group("shaders" FILES ${SHADER_LIST})

add_executable(lab02-euler ${SOURCE_LIST} ${INCLUDE_LIST} ${SHADER_LIST})
target_link_libraries(lab02-euler ${ATLAS_LIBRARIES})

