#version 410 core

uniform sampler2D texture;

in VS_OUT
{
	vec2 tc;
} fs_in;

out vec4 fragColor;

void main() 
{
	vec3 texColor = texture(texture,fs_in.tc).rgb;
	fragColor = vec4(texColor, 1.0);
}
