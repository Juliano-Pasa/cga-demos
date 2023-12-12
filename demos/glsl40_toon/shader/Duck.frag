// Frament Shader //

#version 400

in vec3 gNormal;
in vec3 gVertexColor;
in vec3 gLightDirection;

void main()
{
	float intensity = max(dot(gLightDirection, gNormal), 0.0);
	vec3 changedColor = gVertexColor;

	if (intensity > 0.98)
		changedColor = gVertexColor * vec3(0.9,0.9,0.9);
	else if (intensity > 0.5)
		changedColor = gVertexColor * vec3(0.4,0.4,0.4);	
	else if (intensity > 0.25)
		changedColor = gVertexColor * vec3(0.2,0.2,0.2);
	else
		changedColor = gVertexColor * vec3(0.1,0.1,0.1);	

	changedColor = gVertexColor * vec3(intensity, intensity, intensity);

	gl_FragColor = vec4(changedColor, 1.0);
}
