#version 120

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;

attribute vec3 in_position;
attribute vec2 in_texcoord;

varying vec3 position;
varying vec2 texcoord;

void main()
{
	texcoord = in_texcoord;
	gl_Position = MVPMatrix * vec4(in_position, 1.0);
	position = vec3(MVMatrix * vec4(in_position, 1.0));
}
