#version 330 core
out vec4 FragColor;
in vec2 pos; 

uniform sampler2D GradientMap;
uniform float Resolution;

void main()
{
    vec2 p=0.5*(vec2(1.0,1.0)+pos);
    vec4 pixel = texture2D(GradientMap, p);
    float sum=0.0,offset=1.0/Resolution;
    if(pixel.a==0.5){
        FragColor = vec4((texture2D(GradientMap, vec2(p.x-offset,p.y-offset)) + texture2D(GradientMap, vec2(p.x-offset,p.y+offset)) + texture2D(GradientMap, vec2(p.x+offset,p.y-offset)) + texture2D(GradientMap, vec2(p.x+offset,p.y+offset))/4.0).rgb,pixel.a);
    }
    else{
        FragColor = pixel;
    }
}
