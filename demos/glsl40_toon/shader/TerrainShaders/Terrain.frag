// Fragment Shader //

#version 400

in vec3 geoLightDirection;
in vec3 geoNormal;
in vec3 geoColor;

uniform vec3 color;

void main()
{
	float intensity = max(dot(geoLightDirection, geoNormal), 0.0);
	vec3 changedColor = geoColor;

	if (intensity > 0.98)
		changedColor = color * vec3(0.9,0.9,0.9);
	else if (intensity > 0.5)
		changedColor = color * vec3(0.4,0.4,0.4);	
	else if (intensity > 0.25)
		changedColor = color * vec3(0.2,0.2,0.2);
	else
		changedColor = color * vec3(0.1,0.1,0.1);	

	changedColor = geoColor * vec3(intensity, intensity, intensity);

	gl_FragColor = vec4(changedColor, 1.0);
}