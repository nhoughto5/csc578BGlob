#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
uniform mat4 mvpMat;

void main()
{
	gl_Position = mvpMat * vec4(vPosition, 1.0);
}