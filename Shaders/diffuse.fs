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
        vec4 A = texture2D(GradientMap, vec2(p.x-offset,p.y-offset));
        vec4 B = texture2D(GradientMap, vec2(p.x-offset,p.y+offset));
        vec4 C = texture2D(GradientMap, vec2(p.x+offset,p.y-offset));
        vec4 D = texture2D(GradientMap, vec2(p.x+offset,p.y+offset));
        vec4 sum = (A+B+C+D)/4.0;
        float length = sqrt(sum.r*sum.r+sum.g*sum.g);
        if(length!=0){
            //FragColor = vec4(0.0,0.0,sum.b,pixel.a);
            FragColor = vec4(sum.rg/length,sum.b,pixel.a);
        }
    }
    else{
        FragColor = pixel;
    }
}
