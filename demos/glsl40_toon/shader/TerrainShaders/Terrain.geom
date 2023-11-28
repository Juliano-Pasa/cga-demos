// Geometry Shader //

#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 54) out;

in vec3 tesLightDirection[];
in vec3 tesVertexColor[];

uniform mat4 VP;
uniform vec3 dirtColor;
uniform int patchWidth;
uniform int tessLevel;

out vec3 geoNormal;
out vec3 geoColor;
out vec3 geoLightDirection;

vec3 CalculateNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(b, a));
}

float RandHash(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 GetTriangleCenter()
{
	vec4 newVertex = gl_in[0].gl_Position +
					 gl_in[1].gl_Position +
					 gl_in[2].gl_Position;
	
	return newVertex / 3.0f;
}

void DrawGrassSector(float height, vec3 normal)
{
	float maxLight = 32.0f;
	float lightAttenuation = height / maxLight;
	vec4 normalDisplacement = vec4(normal, 0.0) * height;

	gl_Position = VP * (gl_in[0].gl_Position + normalDisplacement);
	geoNormal = normal;
	geoColor = tesVertexColor[0] * lightAttenuation;
	geoLightDirection = tesLightDirection[0];
	EmitVertex();

	gl_Position = VP * (gl_in[1].gl_Position + normalDisplacement);
	geoNormal = normal;
	geoColor = tesVertexColor[1] * lightAttenuation;
	geoLightDirection = tesLightDirection[1];
	EmitVertex();

	gl_Position = VP * (gl_in[2].gl_Position  + normalDisplacement);
	geoNormal = normal;
	geoColor = tesVertexColor[2] * lightAttenuation;
	geoLightDirection = tesLightDirection[2];
	EmitVertex();
	EndPrimitive();
}

void GenerateGrass(vec2 centerPoint, vec3 normal)
{
	int iterations = 32;
	float height = 3.0f;

	float rand = RandHash(centerPoint) * (iterations - 1) * height;

	while (rand > height)
	{
		DrawGrassSector(height, normal);
		height += 3.0f;
	}
}

void main()
{
	vec3 normalVector = CalculateNormal();

	gl_Position = VP * gl_in[0].gl_Position;
	geoNormal = normalVector;
	geoColor = dirtColor;
	geoLightDirection = tesLightDirection[0];
	EmitVertex();

	gl_Position = VP * gl_in[1].gl_Position;
	geoNormal = normalVector;
	geoColor = dirtColor;
	geoLightDirection = tesLightDirection[1];
	EmitVertex();

	gl_Position = VP * gl_in[2].gl_Position;
	geoNormal = normalVector;
	geoColor = dirtColor;
	geoLightDirection = tesLightDirection[2];
	EmitVertex();
	EndPrimitive();

	float squareSize = float(patchWidth) / float(tessLevel);

	if (squareSize > 31)
	{
	vec4 triangleCenter = GetTriangleCenter();
	vec2 roundedCenter = vec2(floor(triangleCenter.x / squareSize), floor(triangleCenter.z / squareSize));

	GenerateGrass(roundedCenter, normalVector);	
	}
}
