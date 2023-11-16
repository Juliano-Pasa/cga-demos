// Geometry Shader //

#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 10) out;

in vec3 tesLightDirection[];

uniform mat4 MVP;

out vec3 geoNormal;
out vec3 geoLightDirection;

vec3 CalculateNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(b, a));
}

void DrawNormal(vec3 normal)
{
	gl_Position = MVP * gl_in[0].gl_Position;
	geoNormal = normal;
	geoLightDirection = tesLightDirection[0];
	EmitVertex();

	gl_Position = MVP * (gl_in[0].gl_Position + vec4(normal, 0.0));
	geoNormal = normal;
	geoLightDirection = tesLightDirection[0];
	EmitVertex();
	EndPrimitive();
}

void main()
{
	vec3 normalVector = CalculateNormal();

	gl_Position = MVP * gl_in[0].gl_Position;
	geoNormal = normalVector;
	geoLightDirection = tesLightDirection[0];
	EmitVertex();

	gl_Position = MVP * gl_in[1].gl_Position;
	geoNormal = normalVector;
	geoLightDirection = tesLightDirection[1];
	EmitVertex();

	gl_Position = MVP * gl_in[2].gl_Position;
	geoNormal = normalVector;
	geoLightDirection = tesLightDirection[2];
	EmitVertex();
	EndPrimitive();
}
