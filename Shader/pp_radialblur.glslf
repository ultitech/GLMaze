#version 120
#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect image;
uniform ivec2 screen_size;
uniform vec2 gaussValues[11];

void main()
{
	vec2 circle_pos = (gl_FragCoord.xy / screen_size) * 2.0 - 1.0;
	float len = length(circle_pos);
	
	vec4 color;
	for(int i=0; i<11; i++)
	{
		color += texture2DRect(image, gl_FragCoord.xy + circle_pos * len * 5 * gaussValues[i].x) * gaussValues[i].y;
	}
	
	gl_FragColor = color;
}
