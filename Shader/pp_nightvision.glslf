#version 150

uniform samplerRect image;
uniform ivec2 screen_size;

out vec4 out_color;

void main()
{
	vec3 color = texture(image, gl_FragCoord.xy).rgb;
	color += vec3(0.5, 0.5, 0.5);
	float fac = mod(gl_FragCoord.y, 4.0) > 2.0 ? 1.5 : 1.0;
	color *= vec3(0.0, fac, 0.0);
	
	vec2 circle_pos = (gl_FragCoord.xy / screen_size) * 2.0 - 1.0;
	float len = length(circle_pos);
	color *= 1.0 - len*len*0.5;
	
	out_color = vec4(color, 1.0);
}
