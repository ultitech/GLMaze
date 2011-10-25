#version 120

uniform mat4 MVPMatrix;

attribute vec3 in_position;

void main()
{
	gl_Position = MVPMatrix * vec4(in_position, 1.0);
}
