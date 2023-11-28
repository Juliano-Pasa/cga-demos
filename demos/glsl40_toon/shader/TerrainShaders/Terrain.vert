// Vertex Shader //

# version 400

layout(location = 0) in vec3 vertexPosition;

uniform mat4 M;

void main()
{
	gl_Position = M * vec4(vertexPosition, 1.0);
}