set(SHADER_SOURCE
    "${GLOB_SHADER_ROOT}/grid.fs.glsl"
    "${GLOB_SHADER_ROOT}/grid.vs.glsl"
	"${GLOB_SHADER_ROOT}/glob.fs.glsl"
    "${GLOB_SHADER_ROOT}/glob.vs.glsl"
    "${GLOB_SHADER_ROOT}/spline.vs.glsl"
    "${GLOB_SHADER_ROOT}/spline.fs.glsl"
    )

set_source_files_properties(${SHADER_SOURCE} 
    PROPERTIES HEADER_FILE_ONLY TRUE)

set(SHADER_INCLUDE "${GLOB_GENERATED_ROOT}/ShaderPaths.hpp")

configure_file(
    "${GLOB_SHADER_ROOT}/ShaderPaths.hpp.in"
    ${SHADER_INCLUDE})

set(SHADER_LIST
    ${SHADER_SOURCE}
    ${SHADER_INCLUDE}
    PARENT_SCOPE)