set(SHADER_SOURCE
    "${LAB01_SHADER_ROOT}/triangle.fs.glsl"
    "${LAB01_SHADER_ROOT}/triangle.vs.glsl")

set_source_files_properties(${SHADER_SOURCE} 
    PROPERTIES HEADER_FILE_ONLY TRUE)

set(SHADER_INCLUDE "${LAB01_GENERATED_ROOT}/ShaderPaths.hpp")

configure_file(
    "${LAB01_SHADER_ROOT}/ShaderPaths.hpp.in"
    ${SHADER_INCLUDE})

set(SHADER_LIST
    ${SHADER_SOURCE}
    ${SHADER_INCLUDE}
    PARENT_SCOPE)

