#version 430 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    vec3 color = ourColor.rgb;
    vec2 n = vec2(0);

    float length = sqrt(ourColor.x * ourColor.x + ourColor.z * ourColor.z);
    if (length == 0){
        color.y = 0;
    }
    else{
        color = normalize(color);
        color.y = color.y / length;
        n = normalize(color.xz);
    }
    FragColor = vec4(n,color.y, 1.0f);
}