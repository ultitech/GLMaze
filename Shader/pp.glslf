#version 120
#extension GL_ARB_texture_rectangle : require

uniform sampler2DRect Image;

void main()
{
	gl_FragColor = texture2DRect(Image, gl_FragCoord.xy);
}
