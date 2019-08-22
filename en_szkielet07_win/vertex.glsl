#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Attributes
in vec4 vertex; //vertex coordinates in model space
in vec4 normal; //normal vector coordinates in model space
//in vec4 color; //color associated with the vertex

//Varying variables
out vec4 ic;

void main(void) {

    vec4 kd=vec4(0.9f,0.9f,0.9f,1.0f) ; //Object color
    vec4 ld=vec4(1,1,1,1); //Light color

    vec4 lp=vec4(0,0.5,0.1,1);//Light source location in world space

    vec4 l=normalize(V*(lp-M*vertex)); //Light vector in eye space
    vec4 n=normalize(V*M*normal); //Normal vector in eye space

    float nl=clamp(dot(n,l),0,1);

    ic=vec4(kd.rgb*nl,kd.a);
    gl_Position=P*V*M*vertex;
}
