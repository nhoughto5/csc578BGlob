#version 330 core
in vec3 theNormal;
in vec3 thePosition;
out vec4 colour;
uniform vec3 lightPosition;
void main()
{
	vec3 lightVector = normalize(lightPosition - thePosition);
	float brightness = dot(lightVector, theNormal);
	vec3 theColor = vec3(1, 0, 0) * brightness;
	colour = vec4(theColor, 1);
}