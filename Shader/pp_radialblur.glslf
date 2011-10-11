#version 150

uniform samplerRect image;
uniform ivec2 screen_size;
uniform vec2 gaussValues[11];

out vec4 out_color;

void main()
{
	vec2 circle_pos = (gl_FragCoord.xy / screen_size) * 2.0 - 1.0;
	float len = length(circle_pos);
	
	vec4 color;
	for(int i=0; i<11; i++)
	{
		color += texture(image, gl_FragCoord.xy + circle_pos * len * 5 * gaussValues[i].x) * gaussValues[i].y;
	}
	
	out_color = color;
}
