#version 330 core
out vec4 FragColor;
in vec4 ourColor;
void main()
{
    if(ourColor.r>0.0f){
        FragColor = vec4(0.0,ourColor.r,ourColor.ba);
    }
    else{
        FragColor = ourColor;
    }
    
}