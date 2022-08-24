#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 ourColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position =  projection * view * mat4(1.0) * vec4(aPos.x,aPos.y,aPos.z, 1.0f);
    ourColor = aNormal;
}