// Geometry Shader


layout (triangles) in;

layout (points, max_vertices = 0) out;

out vec3 normalVector;

void main()
{
	vec4 u = gl_in[1].gl_Position - gl_in[0].gl_Position;
	vec4 v = gl_in[2].gl_Position - gl_in[0].gl_Position;

	vec3 n = vec3(1.0);
	n.x = u.y*v.z - u.z*v.y;
	n.y = u.z*v.x - u.x*v.z;
	n.z = u.x*v.y - u.y*v.x;
}