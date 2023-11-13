// Vertex Shader //

# version 400

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 inTextureCoordinate;

out vec2 vTextureCoordinate;

void main()
{
	gl_Position = vec4(vertexPosition, 1.0);
	vTextureCoordinate = inTextureCoordinate;
}