/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#version 120

void main()
{
	if(!gl_FrontFacing) discard;
	gl_FragColor = vec4(vec3(1.0), 0.7);
}
