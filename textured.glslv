#version 150

uniform mat4 MVP;

in vec3 in_position;
in vec2 in_texcoord;

out vec2 texcoord;

void main()
{
	texcoord = in_texcoord;
	gl_Position = MVP * vec4(in_position, 1.0);
}
