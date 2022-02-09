#version 330 core
out vec4 FragColor;
in vec4 ourColor;
void main()
{
    if(ourColor.w==1){
        FragColor = vec4(0.0f,1.0f,0.0f, 0.0f);
    }
    else{
        FragColor = ourColor;
    }
}