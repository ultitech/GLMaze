#version 150

uniform sampler2D diffuse;

in vec3 position;
in vec2 texcoord;

out vec4 out_color;

void main()
{
	vec3 tex_color = texture(diffuse, texcoord).rgb;
	float x = length(position);
	float intensity = 1.0/(1.1+3.0*x*x) + 0.1;
	out_color = vec4(tex_color*intensity, 1.0);
}
