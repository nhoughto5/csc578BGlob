set(SHADER_SOURCE
    "${LAB04_SHADER_ROOT}/grid.fs.glsl"
    "${LAB04_SHADER_ROOT}/grid.vs.glsl"
    "${LAB04_SHADER_ROOT}/ball.vs.glsl"
    "${LAB04_SHADER_ROOT}/ball.fs.glsl"
    "${LAB04_SHADER_ROOT}/spline.vs.glsl"
    "${LAB04_SHADER_ROOT}/spline.fs.glsl"
    )

set_source_files_properties(${SHADER_SOURCE} 
    PROPERTIES HEADER_FILE_ONLY TRUE)

set(SHADER_LIST
    ${SHADER_SOURCE}
    PARENT_SCOPE)
