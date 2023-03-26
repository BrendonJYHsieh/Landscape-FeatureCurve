#version 430 core
layout (location = 0) in vec2 aPos;

out vec2 pos;   // screen position <-1,+1>

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position =  projection * view * model * vec4(aPos.x,0.0f,aPos.y, 1.0f);
    pos = aPos.xy;
}