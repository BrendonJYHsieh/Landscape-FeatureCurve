#version 330 core
out vec4 FragColor;
in vec2 pos; 

uniform sampler2D ElevationMap;
uniform sampler2D GradientMap;
uniform float Resolution;

void main()
{
    vec2 p=0.5*(vec2(1.0,1.0)+pos);
    vec4 pixel = texture2D(ElevationMap, p);
    if(pixel.a==0){
        FragColor = vec4(0.0,0.0,0.0,0.0);
    }
    else{
        FragColor = texture2D(GradientMap, p);
    }
}
