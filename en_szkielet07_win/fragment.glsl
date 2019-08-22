#version 330


//varying variables
in vec4 ic;

//output variable
out vec4 pixelColor; //Fragment shader output variable. Represents computed pixel color.

void main(void) {
	pixelColor=ic;
}
