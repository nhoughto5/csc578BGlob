#version 330 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vNormal;
layout(location = 2) in vec3 vColor;

uniform mat4 mvpMat;
out vec3 color;
void main()
{
    gl_Position = mvpMat * vPosition;
	color = vColor;
}