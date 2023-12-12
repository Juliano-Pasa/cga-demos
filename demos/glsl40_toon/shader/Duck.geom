// Geometry Shader //

#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vVertexColor[];

uniform vec3 lightPos;
uniform mat4 VP;

out vec3 gVertexColor;
out vec3 gLightDirection;
out vec3 gNormal;

vec3 CalculateNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(b, a));
}

vec3 CalculateLightDirection(vec4 vertexPosition)
{
	vec3 direction = lightPos - vertexPosition.xyz;
	return normalize(direction);
}

void main()
{
	vec3 normalVector = CalculateNormal();

	gNormal = normalVector;
	gVertexColor = vVertexColor[0];
	gLightDirection = CalculateLightDirection(gl_in[0].gl_Position);
	gl_Position = VP * gl_in[0].gl_Position;
	EmitVertex();

	gNormal = normalVector;
	gVertexColor = vVertexColor[1];
	gLightDirection = CalculateLightDirection(gl_in[1].gl_Position);
	gl_Position = VP * gl_in[1].gl_Position;
	EmitVertex();

	gNormal = normalVector;
	gVertexColor = vVertexColor[2];
	gLightDirection = CalculateLightDirection(gl_in[2].gl_Position);
	gl_Position = VP * gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}

