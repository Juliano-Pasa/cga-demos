// Fragment Shader //

#version 400

in vec3 lightDirection;
in vec3 normalVector;

uniform vec3 color;

void main()
{
	float intensity = max(dot(lightDirection, normalVector), 0.0);
	vec3 changedColor = color;

	if (intensity > 0.98)
		changedColor = color * vec3(0.9,0.9,0.9);
	else if (intensity > 0.5)
		changedColor = color * vec3(0.4,0.4,0.4);	
	else if (intensity > 0.25)
		changedColor = color * vec3(0.2,0.2,0.2);
	else
		changedColor = vec3(1.0, 0.0, 0.0) * vec3(0.1,0.1,0.1);	
		
	changedColor = color * vec3(intensity, intensity, intensity);

	if (color.x < 0.35)
	{
		changedColor = color;
	}

	gl_FragColor = vec4(changedColor, 1.0);
}