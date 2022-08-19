#version 330 core

out vec4 FragColor;
in vec2 pos; 

uniform sampler2D F;
uniform sampler2D E;
uniform sampler2D G;
uniform float Resolution;


float sign(float n){
    if (n > 0) {
		return 1.0/Resolution;
	}
	else if (n < 0) {
		return -1.0/Resolution;
	}
	else {
		return 0.0;
	}
}


void main()
{
    float a=0,b=0,f=0;
    float nx=0,ny=0;
    float FL=0,FN=0,FI=0;
    vec2 p=0.5*(vec2(1.0,1.0)+pos);

    float sum=0.0,offset=1.0/Resolution;

    vec4 F_pixel = texture2D(F, p);
    vec4 E_pixel = texture2D(E, p);
    vec4 G_pixel = texture2D(G, p);

    if (E_pixel.a == 0) {
        a = 0;
        b = 0;
    }
    else {
        a = E_pixel.a;
        b = 1.0 - a;
    }

    vec4 A = texture2D(F, vec2(p.x-offset,p.y-offset));
    vec4 B = texture2D(F, vec2(p.x-offset,p.y+offset));
    vec4 C = texture2D(F, vec2(p.x+offset,p.y-offset));
    vec4 D = texture2D(F, vec2(p.x+offset,p.y+offset));

    float Gradient = sqrt(((A.r-B.r)*(A.r-B.r) + (C.r-D.r)*(C.r-D.r))*G_pixel.b*G_pixel.b);
    //float Gradient=0;

    FL = (A+B+C+D).r/4.0;
   
    nx = G_pixel.r;
    ny = G_pixel.g;

    FN = (nx * nx * texture2D(F, vec2(p.x-sign(nx),p.y)).r + ny * ny * texture2D(F, vec2(p.x,p.y-sign(ny))).r) * G_pixel.b;

    FI = E_pixel.r;

    f = a * FL + b * FN + (1.0-a-b) * FI;
    FragColor = vec4(f,0.0,0.0,0.0);
}