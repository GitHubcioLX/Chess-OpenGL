#version 330


//Varying variables
in vec4 l;
in vec4 l2;
in vec4 n;
in vec4 v;
in vec2 iTexCoord0;
in vec2 iTexCoord1;

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

out vec4 pixelColor; //Fragment shader output variable

void main(void) {
    vec4 kd=mix(texture(textureMap0,iTexCoord0),texture(textureMap1,iTexCoord1),0.15); //Material color for a diffused light
    vec4 ld=vec4(1,1,1,1); //Color of the diffused light
    vec4 ks=vec4(1,1,1,1); //Material color for a reflected light
    vec4 ls=vec4(1,1,1,1); //Color of the reflected light

    vec4 ml=normalize(l);
    vec4 ml2=normalize(l2);
    vec4 mn=normalize(n);
    vec4 mv=normalize(v);
    vec4 mr=reflect(-ml,mn); //Reflection direction vector in the eye space
    vec4 mr2=reflect(-ml2,mn);

    float nl=clamp(dot(mn,ml),0,1); //cosine of the angle between n and l vectors
    float nl2=0.5*clamp(dot(mn,ml2),0,1);
    float rv=pow(clamp(dot(mr,mv),0,1),25); //cosine of the angle between r and v vectors to the power of Phong exponent
    float rv2=0.5*pow(clamp(dot(mr2,mv),0,1),25);

	pixelColor=vec4(kd.rgb*ld.rgb*(nl+nl2)+ks.rgb*ls.rgb*(rv+rv2),kd.a);
}
