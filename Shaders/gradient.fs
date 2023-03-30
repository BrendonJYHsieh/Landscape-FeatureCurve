#version 430 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    
    vec3 color = ourColor.rgb;
    float length = sqrt(ourColor.x * ourColor.x + ourColor.y * ourColor.y);
    
    if (length == 0){
        color.z = 0;
    }
    else{
        color = normalize(color);
        color.z = color.z / length;
    }

    FragColor = vec4(normalize(color.xy),color.z, 1.0f);
}