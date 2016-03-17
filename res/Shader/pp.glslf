/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#version 120
#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect Image;

void main()
{
	gl_FragColor = texture2DRect(Image, gl_FragCoord.xy);
}
