# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.0)

project(uvic-animation)

# Tell IDE to use folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#================================
# Directory variables.
#================================
set(ANIM_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(ANIM_BINARY_DIR "${PROJECT_BINARY_DIR}")
set(ANIM_LIB_ROOT "${ANIM_SOURCE_DIR}/lib")

#================================
# Compilation flags.
#================================
if (MSVC)
    add_definitions(
        -wd4201 # nonstandard extension used.
        -D_CRT_SECURE_NO_WARNINGS
        )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

    # Clang requires an additional library to be linked to use C++11.
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

#================================
# Add subdirectories.
#================================
# Add Atlas
add_subdirectory("${ANIM_LIB_ROOT}/atlas")

# Now add the lab subdirectories.
add_subdirectory("${ANIM_SOURCE_DIR}/lab01-intro")
add_subdirectory("${ANIM_SOURCE_DIR}/lab02-euler")

