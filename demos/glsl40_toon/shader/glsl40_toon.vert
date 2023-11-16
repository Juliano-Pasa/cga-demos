// simple toon vertex shader
// www.lighthouse3d.com

#version 400

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexColor;
layout (location=2) in vec3 VertexNormal;

out vec3 Color;
out vec3 Normal;
out vec4 lightDir;

uniform mat4 MVP;
uniform mat3 NormalMatrix;
uniform mat4 ModelMatrix;
uniform vec3 lightPos;

void main()
{
    Color = VertexColor;
	Normal = normalize(VertexNormal);

	vec4 position = ModelMatrix * vec4(VertexPosition, 1.0);
	lightDir = normalize(vec4(lightPos, 1.0) - position);

	gl_Position = MVP * vec4(VertexPosition, 1.0);
}
