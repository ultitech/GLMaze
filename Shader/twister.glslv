#version 150

uniform mat4 MVPMatrix;

in vec3 in_position;

void main()
{
	gl_Position = MVPMatrix * vec4(in_position, 1.0);
}
