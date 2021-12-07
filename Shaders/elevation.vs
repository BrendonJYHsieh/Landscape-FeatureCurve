#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	//gl_Position =  projection * view * model * vec4(aPos, 1.0f);
    gl_Position =  projection * view * model * vec4(aPos.x,0.1f,aPos.y, 1.0f);
    ourColor = vec3(aPos.z/255,0.0f,0.0f);
}