// Geometry Shader

#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 normalVectorTes[3];
in vec3 viewDirectionTes[3];
in vec3 lightDirectionTes0[3];
in vec3 lightDirectionTes1[3];

out vec3 normalVector;
out vec3 viewDirection;
out vec3 lightDirection0;
out vec3 lightDirection1;

const int mode = 0;

// O geometry shader nesse caso so esta repassando os valores
void DrawWaves()
{
	gl_Position = gl_in[0].gl_Position;
	normalVector = normalVectorTes[0];
	viewDirection = viewDirectionTes[0];
	lightDirection0 = lightDirectionTes0[0];
	lightDirection1 = lightDirectionTes1[0];
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	normalVector = normalVectorTes[1];
	viewDirection = viewDirectionTes[1];
	lightDirection0 = lightDirectionTes0[1];
	lightDirection1 = lightDirectionTes1[1];
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	normalVector = normalVectorTes[2];
	viewDirection = viewDirectionTes[2];
	lightDirection0 = lightDirectionTes0[2];
	lightDirection1 = lightDirectionTes1[2];
	EmitVertex();

	EndPrimitive();
}

void main()
{	
	if (mode == 0)
	{
		DrawWaves();
	}
	else if (mode == 1)
	{
		DrawWaves();
	}
}