#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 ourColor;
uniform sampler2D Texture;
uniform float maxHeight = 0; 
uniform float minHeight = 0;

void main()
{
    float range = maxHeight - minHeight;
    float delta = ourColor.r - minHeight;
    delta /= range;
    
    /*
    if(ourColor.r>0.0f){
        FragColor = vec4(0.0,ourColor.r/255.0,0.0,0.0);
        //FragColor = vec4(texture(Texture,TexCoords).rgb,1.0);
    }
    else{
        FragColor = ourColor;
    }
    */

    if(ourColor.r == 0){
        FragColor = vec4(0.0, 0.0, 0, 0);
    }else{
        FragColor = vec4(delta, 0, 1.0 - delta, 0);
    }
}