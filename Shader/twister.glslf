#version 150

out vec4 out_color;

void main()
{
	if(!gl_FrontFacing) discard;
	out_color = vec4(vec3(1.0), 0.7);
}
