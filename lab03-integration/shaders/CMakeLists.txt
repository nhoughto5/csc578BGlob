set(SHADER_SOURCE
    "${LAB03_SHADER_ROOT}/grid.fs.glsl"
    "${LAB03_SHADER_ROOT}/grid.vs.glsl"
    "${LAB03_SHADER_ROOT}/ball.vs.glsl"
    "${LAB03_SHADER_ROOT}/ball.fs.glsl"
    )

set_source_files_properties(${SHADER_SOURCE} 
    PROPERTIES HEADER_FILE_ONLY TRUE)

set(SHADER_INCLUDE "${LAB03_GENERATED_ROOT}/ShaderPaths.hpp")

configure_file(
    "${LAB03_SHADER_ROOT}/ShaderPaths.hpp.in"
    ${SHADER_INCLUDE})

set(SHADER_LIST
    ${SHADER_SOURCE}
    ${SHADER_INCLUDE}
    PARENT_SCOPE)

