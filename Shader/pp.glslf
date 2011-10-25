#version 120
#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect image;

void main()
{
	gl_FragColor = texture2DRect(image, gl_FragCoord.xy);
}
