#version 330 core
layout (location = 0) in vec3 aPos;
out vec2 pos;   // screen position <-1,+1>

void main()
{
    gl_Position = vec4(aPos.x,aPos.y,aPos.z, 1.0f);
    pos = aPos.xy;
}