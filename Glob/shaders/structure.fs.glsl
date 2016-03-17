#version 330 core
in vec3 theNormalWorld;
in vec3 vPositionWorld;
in vec3 colour;
out vec4 outColour;
uniform vec3 lightPositionWorld;
uniform vec4 ambientLight;
uniform vec3 eyePositionWorld;
void main()
{	
	vec3 theColour = vec3(0.0, 0.0, 1.0f);

	//Diffuse Light
	vec3 lightVectorWorld = normalize(lightPositionWorld - vPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(theNormalWorld));
	vec4 diffuseLight = vec4(colour * brightness, 1.0f);

	//Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, theNormalWorld);
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vPositionWorld);
	float s = dot(reflectedLightVectorWorld, eyeVectorWorld);
	s = pow(s, 100);
	vec4 specularLight = vec4(s, s, s, 1);


	vec4 ambientColor = ambientLight * vec4(colour, 1);
	outColour = ambientColor + clamp(diffuseLight, 0, 1)+ 0.5 * clamp(specularLight, 0, 1);
}