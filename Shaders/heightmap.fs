#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 ourColor;
uniform sampler2D Texture;


void main()
{
    if(ourColor.r>0.0f){
        //FragColor = vec4(0.0,ourColor.r,ourColor.ba);
        FragColor = vec4(texture(Texture,TexCoords).rgb,1.0);
    }
    else{
        FragColor = ourColor;
    }
    
}