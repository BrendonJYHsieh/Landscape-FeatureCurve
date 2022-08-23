#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    vec3 color = ourColor.rgb;
    float length = sqrt(ourColor.x * ourColor.x + ourColor.z * ourColor.z);
    if(length == 0){
        color.y = 0;
    }
    else{
        color.y = color.y / length;
    }
    FragColor = vec4(color.xz,color.y, 1.0f);
}