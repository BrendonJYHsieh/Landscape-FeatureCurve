#version 330 core
out vec4 FragColor;
in vec3 ourColor;
void main()
{
    FragColor = vec4(0.0f,ourColor.g,0.0f, 1.0f);
}