#version 150

uniform sampler2D diffuse;

in vec2 texcoord;

out vec4 out_color;

void main()
{
	out_color = vec4(texture(diffuse, texcoord).rgb, 1.0);
}
