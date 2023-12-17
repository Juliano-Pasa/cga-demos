// Frament Shader //

#version 400

in vec3 gNormal;
in vec3 gVertexColor;
in vec3 gLightDirection;

void main()
{
	float intensity = max(dot(gLightDirection, gNormal), 0.0);
	vec3 changedColor = gVertexColor;

	if (intensity > 0.75)
		changedColor = gVertexColor * vec3(1);
	else if (intensity > 0.60)
		changedColor = gVertexColor * vec3(0.9);	
	else if (intensity > 0.45)
		changedColor = gVertexColor * vec3(0.8);
	else if (intensity > 0.30)
		changedColor = gVertexColor * vec3(0.7);
	else if (intensity > 0.15)
		changedColor = gVertexColor * vec3(0.6);
	else
		changedColor = gVertexColor * vec3(0.5);

	gl_FragColor = vec4(changedColor, 1.0);
}
