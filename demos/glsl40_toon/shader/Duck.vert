// Vertex Shader //

# version 400

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

uniform mat4 M;

out vec3 vVertexColor;

void main()
{
	gl_Position = M * vec4(vertexPosition, 1.0);
	vVertexColor = vertexColor;
}