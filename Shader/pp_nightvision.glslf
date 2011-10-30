#version 120
#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect image;
uniform sampler2D noise;
uniform ivec2 screen_size;

void main()
{
	float color = texture2DRect(image, gl_FragCoord.xy).g;
	color += 0.5;
	vec2 texcoord = gl_FragCoord.xy / vec2(screen_size) * 16.0;
	float noise_fac = texture2D(noise, texcoord).a * 0.2;
	float line_fac = mod(gl_FragCoord.y, 4.0) > 2.0 ? 1.5 : 1.0;
	color *= line_fac+noise_fac;
	
	vec2 circle_pos = (gl_FragCoord.xy / screen_size) * 2.0 - 1.0;
	float len = length(circle_pos);
	color *= 1.0 - len*len*0.5;
	
	gl_FragColor = vec4(0.0, color, 0.0, 1.0);
}
