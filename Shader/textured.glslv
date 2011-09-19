#version 150

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;

in vec3 in_position;
in vec2 in_texcoord;

out vec3 position;
out vec2 texcoord;

void main()
{
	texcoord = in_texcoord;
	gl_Position = MVPMatrix * vec4(in_position, 1.0);
	position = vec3(MVMatrix * vec4(in_position, 1.0));
}
