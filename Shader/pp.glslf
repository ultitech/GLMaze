#version 150

uniform samplerRect image;

out vec4 out_color;

void main()
{
	out_color = texture(image, gl_FragCoord.xy);
}
