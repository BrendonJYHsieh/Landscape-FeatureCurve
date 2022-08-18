#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D texture_d;

out vec2 TexCoords;
out vec4 ourColor;

void main()
{
    TexCoords = texture_coordinate;
    vec3 height_map = position;
    height_map.y = height_map.y + (texture(texture_d,texture_coordinate).r)*255.0f;
	gl_Position =  projection * view * model * vec4(height_map, 1.0f);
    ourColor = texture(texture_d,texture_coordinate).rgba;
    
}