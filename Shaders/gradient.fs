#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    vec3 color = ourColor.rgb;
    vec2 n;
    color = normalize(color);
    n = normalize(color.xz);

    float length = sqrt(ourColor.x * ourColor.x + ourColor.z * ourColor.z);
    
    color.y = color.y / length;
    if (length == 0){
        color.y = 0;
    }
    //color.y = abs(color.y);
    FragColor = vec4(n.x,n.y,color.y, 1.0f);
    //FragColor = vec4(0.0, 0.0,color.y, 1.0f);
}