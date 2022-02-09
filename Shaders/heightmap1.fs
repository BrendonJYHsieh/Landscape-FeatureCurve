#version 330 core
out vec4 FragColor;
in vec3 ourColor;
void main()
{
    FragColor = vec4(ourColor.g,ourColor.b,ourColor.b, 0.0f);
}