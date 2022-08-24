#version 330 core
layout (location = 0) in vec4 aPos;

out vec4 ourColor;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    ourColor = vec4(aPos.y,0.0f,0.0f,aPos.w);
    gl_Position =  projection * view * mat4(1.0) * vec4(aPos.x,aPos.y,aPos.z, 1.0f);
}