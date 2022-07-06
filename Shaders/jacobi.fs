#version 330 core

out vec4 FragColor;
in vec2 pos; 

uniform sampler2D F;
uniform sampler2D E;
uniform sampler2D G;

float sign(float n){
    if (n > 0) {
		return 1.0/128.0;
	}
	else if (n < 0) {
		return -1.0/128.0;
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

    float sum=0.0,resolution=1.0/128.0;

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
    FL = (texture2D(F, vec2(p.x-resolution,p.y-resolution)).r + texture2D(F, vec2(p.x-resolution,p.y+resolution)).r + texture2D(F, vec2(p.x+resolution,p.y-resolution)).r + texture2D(F, vec2(p.x+resolution,p.y+resolution)).r)/4.0;
    nx = G_pixel.r * G_pixel.b;
    ny = G_pixel.g * G_pixel.b;

    FN = nx * nx * texture2D(F, vec2(p.x-sign(nx),p.y)).r + ny * ny * texture2D(F, vec2(p.x,p.y-sign(ny))).r;

    FI = E_pixel.r;

    f = a * FL + b * FN + (1.0-a-b) * FI;
    FragColor = vec4(f,0.0,0.0,0.0);
}