#version 330 core
layout (location = 0) in vec4 aPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position =  projection * view * mat4(1.0) * vec4(aPos.xyz, 1.0f);
}