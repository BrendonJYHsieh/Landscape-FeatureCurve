#version 330 core
out vec4 FragColor;
in vec4 ourColor;
void main()
{
    if(ourColor.r>0.0f){
        FragColor = vec4(ourColor.r*2.0,ourColor.gba);
    }
    else{
        FragColor = ourColor;
    }
    
}