#version 330 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColour;

uniform mat4 uMVP;
out vec3 fColour;
void main()
{
	gl_Position = uMVP * vec4(vPosition, 1.0);
	fColour = vColour;
}