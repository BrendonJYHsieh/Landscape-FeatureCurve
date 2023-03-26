#version 430 core
out vec4 FragColor;
in vec2 pos;

uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, 0.5*(vec2(1.0,1.0)+pos));
}