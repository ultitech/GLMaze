/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#version 120
#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect Image;
uniform sampler2D Noise;
uniform ivec2 ScreenSize;
uniform float Time;

void main()
{
	float color = texture2DRect(Image, gl_FragCoord.xy).g;
	color *= 5.0;
	vec2 texcoord = gl_FragCoord.xy / vec2(ScreenSize) * 8.0;
	texcoord += vec2(Time*1.618*50.0, Time*0.9*50.0);
	float noise_fac = texture2D(Noise, texcoord).a * 0.5;
	float line_fac = mod(gl_FragCoord.y, 4.0) > 2.0 ? 1.5 : 1.0;
	color *= line_fac+noise_fac;
	
	vec2 circle_pos = (gl_FragCoord.xy / ScreenSize) * 2.0 - 1.0;
	float len = length(circle_pos);
	color *= 1.0 - len*len*0.5;
	
	gl_FragColor = vec4(0.0, color, 0.0, 1.0);
}
