#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    if(ourColor.r==0&&ourColor.g==0)
    FragColor = vec4(ourColor.r,ourColor.g,0.0f, 0.0f);
    else
    FragColor = vec4(ourColor.r,ourColor.g,0.0f, 0.5f);
}