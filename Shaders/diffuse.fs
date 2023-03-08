#version 330 core
out vec4 FragColor;
in vec2 pos; 

uniform sampler2D GradientMap;
uniform float Resolution;

void main()
{
    vec2 p=0.5*(vec2(1.0,1.0)+pos);
    vec4 pixel = texture2D(GradientMap, p);
    float offset=1.0/Resolution;

    if(pixel.a==0.5){
        
        vec4 left = texture2D(GradientMap, vec2(p.x-offset,p.y));
        vec4 right = texture2D(GradientMap, vec2(p.x+offset,p.y));
        vec4 top  = texture2D(GradientMap, vec2(p.x,p.y+offset));
        vec4 bottom = texture2D(GradientMap, vec2(p.x,p.y-offset));
        vec4 center = texture2D(GradientMap, vec2(p.x,p.y));

        vec4 sum = center*4 - (left+right+top+bottom);
        float length = sqrt(sum.r*sum.r+sum.g*sum.g);
        if(length!=0){
            FragColor = vec4(normalize(sum.rg),sum.b,pixel.a);
        }
        else{
            FragColor = pixel;
        }

    }
    else{
        FragColor = pixel;
    }
}
