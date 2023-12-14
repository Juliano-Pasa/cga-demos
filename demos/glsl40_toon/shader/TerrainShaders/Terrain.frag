// Fragment Shader //

#version 400

in vec3 geoLightDirection;
in vec3 geoNormal;
in vec3 geoColor;

void main()
{
	float intensity = max(dot(geoLightDirection, geoNormal), 0.0);
	vec3 changedColor = geoColor * vec3(intensity, intensity, intensity);

	gl_FragColor = vec4(changedColor, 1.0);
}