#version 150

uniform mat4 MVP;

in vec3 in_position;

void main()
{
	gl_Position = MVP * vec4(in_position, 1.0);
}
