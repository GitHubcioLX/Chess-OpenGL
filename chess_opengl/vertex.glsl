#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp; //Light source coordinates in world space
uniform vec4 lp2;

//Attributes
in vec4 vertex; //vertex coordinates in model space
in vec4 normal; //normal vector coordinates in model space
in vec2 texCoord0; //texturing coordinates

//Varying variables
out vec4 l;
out vec4 l2;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;
out vec2 iTexCoord1;

void main(void) {
    l=normalize(V*(lp-M*vertex)); //Vector "towards the light" in the eye space
    l2=normalize(V*(lp2-M*vertex));
    n=normalize(V*M*normal); //Normal vector in the eye space
    v=normalize(vec4(0,0,0,1)-V*M*vertex); //Vector "towards the viewer" in the eye space

    iTexCoord0=texCoord0; //Interpolate texturing coordinates from texCoord0 attribute
    iTexCoord1=(n.xy+1)/2; //Compute and interpolate texturing coordinates for environment mapping

    gl_Position=P*V*M*vertex;
}
