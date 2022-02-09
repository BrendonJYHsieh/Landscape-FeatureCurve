#version 430 core
layout (location = 0) in vec3 aColor;
layout (location = 1) in vec3 texture_coordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;

void main()
{
	gl_Position =  projection * view * model * vec4(texture_coordinate.x,aColor.r,texture_coordinate.y, 1.0f);
    ourColor = aColor;
}