// Geometry Shader //

#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 54) out;

in vec3 tesLightDirection[];
in vec3 tesVertexColor[];

uniform mat4 VP;
uniform vec3 dirtColor;
uniform float patchWidth;
uniform float patchHeight;
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

void DrawGrassSector(float height, vec3 normal, float maxLight)
{
	float lightAttenuation = height / maxLight;
	vec4 normalDisplacement = vec4(normal, 0.0) * height * 3;

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
	int iterations = 16;

	float heightIncrement = 1.0f;
	float currentHeight = heightIncrement;
	float maxGrassHeight = iterations * currentHeight;

	float rand = RandHash(centerPoint);

	while (rand > (currentHeight / maxGrassHeight))
	{
		DrawGrassSector(currentHeight, normal, maxGrassHeight);
		currentHeight += heightIncrement;
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

	vec4 triangleCenter = GetTriangleCenter();
	vec2 roundedCenter = vec2(floor(triangleCenter.x / squareSize), floor(triangleCenter.z / squareSize));

	GenerateGrass(roundedCenter, normalVector);
}
