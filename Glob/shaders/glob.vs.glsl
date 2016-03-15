#version 330 core

layout(location = 0) in vec4 vPositionModel;
layout(location = 1) in vec3 vNormalModel;
uniform mat4 mvpMat;
uniform mat4 modelToWorldMatrix;
out vec3 theNormalWorld;
out vec3 vPositionWorld;
void main()
{
	gl_Position = mvpMat * vPositionModel;
	theNormalWorld = normalize(vec3(modelToWorldMatrix * vec4(vNormalModel, 0.0f)));
	vPositionWorld = vec3(modelToWorldMatrix * vPositionModel);
}