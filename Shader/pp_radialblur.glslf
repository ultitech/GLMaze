/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#version 120
#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect Image;
uniform ivec2 ScreenSize;
uniform vec2 GaussValues[11];

void main()
{
	vec2 circle_pos = (gl_FragCoord.xy / ScreenSize) * 2.0 - 1.0;
	float len = length(circle_pos);
	
	vec4 color;
	for(int i=0; i<11; i++)
	{
		color += texture2DRect(Image, gl_FragCoord.xy + circle_pos * len * 5 * GaussValues[i].x) * GaussValues[i].y;
	}
	
	gl_FragColor = color;
}
