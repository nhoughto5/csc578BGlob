set(SHADER_SOURCE
    "${LAB02_SHADER_ROOT}/grid.fs.glsl"
    "${LAB02_SHADER_ROOT}/grid.vs.glsl"
    "${LAB02_SHADER_ROOT}/cube.fs.glsl"
    "${LAB02_SHADER_ROOT}/cube.vs.glsl"
    )

set_source_files_properties(${SHADER_SOURCE} 
    PROPERTIES HEADER_FILE_ONLY TRUE)

set(SHADER_INCLUDE "${LAB02_GENERATED_ROOT}/ShaderPaths.hpp")

configure_file(
    "${LAB02_SHADER_ROOT}/ShaderPaths.hpp.in"
    ${SHADER_INCLUDE})

set(SHADER_LIST
    ${SHADER_SOURCE}
    ${SHADER_INCLUDE}
    PARENT_SCOPE)

