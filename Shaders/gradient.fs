#version 430 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    vec3 color = ourColor.rgb;
    float length = sqrt(ourColor.x * ourColor.x + ourColor.y * ourColor.y);
    
    if (length == 0){
        color.b = 0;
    }
    else{
        color.rg /= length;
        color.b = color.b * length;
    }

    FragColor = vec4(color, 1.0f);
}