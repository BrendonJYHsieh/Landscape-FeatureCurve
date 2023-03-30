#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D HeightMap;

out vec2 TexCoords;
out vec4 ourColor;

void main()
{
    TexCoords = texture_coordinate;
    vec3 height_map = position;
    height_map.z = height_map.z + (texture(HeightMap,texture_coordinate).r);
	gl_Position =  projection * view * model * vec4(height_map, 1.0f);
    ourColor = texture(HeightMap,texture_coordinate).rgba;
    
}