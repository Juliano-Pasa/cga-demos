// Fragment Shader //

#version 400

uniform vec3 windColor;

void main()
{
	gl_FragColor = vec4(windColor, 1.0);
}