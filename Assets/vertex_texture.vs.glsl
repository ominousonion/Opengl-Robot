#version 410

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 tex_coord;

out VS_OUT
{
	vec2 tc;
} vs_out;

uniform mat4 um4mvp;

void main()
{
	gl_Position = um4mvp * vec4(iv3vertex, 1);
	vs_out.tc = tex_coord;
}




