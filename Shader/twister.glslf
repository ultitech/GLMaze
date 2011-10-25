#version 120

void main()
{
	if(!gl_FrontFacing) discard;
	gl_FragColor = vec4(vec3(1.0), 0.7);
}
