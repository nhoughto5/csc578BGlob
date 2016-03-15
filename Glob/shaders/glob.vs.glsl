#version 330 core

layout(location = 0) in vec3 vPositionModel;
layout(location = 1) in vec3 vNormalModel;
uniform mat4 mvpMat;
uniform mat4 modelToWorldMatrix;
out vec3 theNormal;
out vec3 thePosition;
void main()
{
	gl_Position = mvpMat * vec4(vPositionModel, 1.0);
	theNormal = normalize(vec3(modelToWorldMatrix * vec4(vNormalModel, 0.0f)));
	thePosition = vec3(modelToWorldMatrix * vec4(vPositionModel, 1.0f));
}