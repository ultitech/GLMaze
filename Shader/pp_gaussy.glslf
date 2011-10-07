#version 150

uniform samplerRect image;
uniform vec2 gaussValues[11];

out vec4 out_color;

void main()
{
	vec4 color;
	for(int i=0; i<11; i++)
	{
		color += texture(image, vec2(gl_FragCoord.x, gl_FragCoord.y+gaussValues[i].x)) * gaussValues[i].y;
	}
	out_color = color;
}
