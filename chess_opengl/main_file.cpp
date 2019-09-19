/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "bierka.h"
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "lodepng.h"
#include "shaderprogram.h"
#include "constants.h"


float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians
float viewer_speed=0;
bool reset=0;
float aspectRatio=1;
ShaderProgram *sp; //Pointer to the shader program

GLuint tex0;
GLuint tex1;
GLuint tex2;
GLuint tex3;

bierka * szachownica = new bierka(-1, -1, 0);

//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;
        if (key==GLFW_KEY_S) viewer_speed=2;
        if (key==GLFW_KEY_W) viewer_speed=-2;
        if (key==GLFW_KEY_R) reset=1;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
        if (key==GLFW_KEY_S) viewer_speed=0;
        if (key==GLFW_KEY_W) viewer_speed=0;
        if (key==GLFW_KEY_R) reset=0;
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

//Function for reading texture files
GLuint readTexture(const char* filename) {
  GLuint tex;
  glActiveTexture(GL_TEXTURE0);

  //Read into computers memory
  std::vector<unsigned char> image;   //Allocate data structure for the image
  unsigned width, height;   //Variables, which will be set to image dimensions
  //Read image
  unsigned error = lodepng::decode(image, width, height, filename);

  //Import to graphics card memory
  glGenTextures(1,&tex); //Initialize a handle
  glBindTexture(GL_TEXTURE_2D, tex); //Activate the handle
  //Copy image from computers memory to graphics cards memory
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return tex;
}

//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {

	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	sp=new ShaderProgram("vertex.glsl",NULL,"fragment.glsl");

    tex0=readTexture("board.png");
    tex1=readTexture("nightsky.png");
    tex2=readTexture("black.png");
    tex3=readTexture("white.png");

    szachownica->modelLoader("plane.obj");
}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {

    glDeleteTextures(1,&tex0);
    glDeleteTextures(1,&tex1);
    glDeleteTextures(1,&tex2);
    glDeleteTextures(1,&tex3);

    delete sp;
}

//Drawing procedure
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float viewer_shift, vector <bierka*> bierki) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
        glm::vec3(0.0f,2.5f,5.0f+viewer_shift),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)); //compute view matrix
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 1.0f, 50.0f); //compute projection matrix

    sp->use();//activate shading program
        //Send parameters to graphics card
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
    glUniform4f(sp->u("lp"),0,2,6,1); //Light coordinates in the world space
    glUniform4f(sp->u("lp2"),0,12,0,1);

	float pozX, pozY, pozZ;
	Wsp anim;

    for(int i = 0; i < bierki.size(); i++)
    {
        if(!bierki.at(i)->moving)
        {
            pozX = -1.1f+(2.2/7.0)*bierki.at(i)->getX();
            pozY = 1.1f-(2.2/7.0)*bierki.at(i)->getY();
            pozZ = 0.00005;
        }
        else
        {
            anim = (bierki.at(i)->animacja).at(bierki.at(i)->animacja.size()-1);
            (bierki.at(i)->animacja).pop_back();
            pozX = anim.x;
            pozY = anim.y;
            if(bierki.at(i)->knight || bierki.at(i)->death)
                pozZ = max(anim.z, float(0.00005));
            else
                pozZ = 0.00005;
            if(bierki.at(i)->animacja.size()==0) bierki.at(i)->moving=0;
        }

        glm::mat4 M=glm::mat4(1.0f);
        M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
        M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Compute model matrix
        M=glm::translate(M, glm::vec3(pozX,pozZ,pozY));
        if(!bierki.at(i)->team) M=glm::rotate(M,PI,glm::vec3(0.0f,1.0f,0.0f));

        glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

        float *verts=&bierki.at(i)->vert[0];
        float *normals=&bierki.at(i)->norm[0];
        float *texCoords=&bierki.at(i)->tex[0];
        unsigned int vertexCount=bierki.at(i)->licz_wierz;

        if(!bierki.at(i)->team)
        {
            glUniform1i(sp->u("textureMap0"),0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,tex2);
        }
        else
        {
            glUniform1i(sp->u("textureMap0"),0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,tex3);
        }

        glUniform1i(sp->u("textureMap1"),1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,tex1);

        glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
        glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,verts); //Specify source of the data for the attribute vertex

        glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute normal
        glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0,normals); //Specify source of the data for the attribute normal

        glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute color
        glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,texCoords); //Specify source of the data for the attribute color

        glDrawArrays(GL_TRIANGLES,0,vertexCount); //Draw the object

        glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
        glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
        glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute color
    }

    glm::mat4 M=glm::mat4(1.0f);
    M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
    M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Compute model matrix

    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));

    float *verts=&szachownica->vert[0];
    float *normals=&szachownica->norm[0];
    float *texCoords=&szachownica->tex[0];
    unsigned int vertexCount=szachownica->licz_wierz;

    glUniform1i(sp->u("textureMap0"),0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex0);

    glUniform1i(sp->u("textureMap1"),1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,tex1);

    glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(sp->a("vertex"),4,GL_FLOAT,false,0,verts); //Specify source of the data for the attribute vertex

    glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute normal
    glVertexAttribPointer(sp->a("normal"),4,GL_FLOAT,false,0,normals); //Specify source of the data for the attribute normal

    glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute color
    glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,texCoords); //Specify source of the data for the attribute color

    glDrawArrays(GL_TRIANGLES,0,vertexCount); //Draw the object

    glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
    glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
    glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute color

    glfwSwapBuffers(window); //Copy back buffer to front buffer
}

typedef struct ruch {
    int x1;
    int y1;
    int x2;
    int y2;
} Ruch;

vector<bierka*> initBierki()
{

    vector<bierka*> bierki;

    for(int j = 0; j < 8; j++)
    {
        bierka * Pion = new bierka(j, 1, 1);
        bierki.push_back(Pion);
    }

    for(int j = 0; j < 8; j++)
    {
        bierka * Pion = new bierka(j, 6, 0);
        bierki.push_back(Pion);
    }

    bierka * tempPion = new bierka(0, 0, 0);
    tempPion->modelLoader("pion.obj");
    bierka * tempWieza = new bierka(0, 0, 0);
    tempWieza->modelLoader("tour.obj");
    bierka * tempSkoczek = new bierka(0, 0, 0);
    tempSkoczek->modelLoader("cavalier.obj");
    bierka * tempGoniec = new bierka(0, 0, 0);
    tempGoniec->modelLoader("fou.obj");
    bierka * tempHetman = new bierka(0, 0, 0);
    tempHetman->modelLoader("dame.obj");
    bierka * tempKrol = new bierka(0, 0, 0);
    tempKrol->modelLoader("roi.obj");

    for(int j = 0; j < bierki.size(); j++)
    {
        bierki.at(j)->copy(*tempPion);
    }

    for(int j = 0; j < 2; j+=1)
    {
        bool t = (-1)*j+1;
        bierka * Wieza1 = new bierka(0, j*7, t);
        Wieza1->copy(*tempWieza);
        bierka * Skoczek1 = new bierka(1, j*7, t);
        Skoczek1->copy(*tempSkoczek);
        bierka * Goniec1 = new bierka(2, j*7, t);
        Goniec1->copy(*tempGoniec);
        bierka * Hetman = new bierka(3, j*7, t);
        Hetman->copy(*tempHetman);
        bierka * Krol = new bierka(4, j*7, t);
        Krol->copy(*tempKrol);
        bierka * Goniec2 = new bierka(5, j*7, t);
        Goniec2->copy(*tempGoniec);
        bierka * Skoczek2 = new bierka(6, j*7, t);
        Skoczek2->copy(*tempSkoczek);
        bierka * Wieza2 = new bierka(7, j*7, t);
        Wieza2->copy(*tempWieza);

        Skoczek1->knight = 1;
        Skoczek2->knight = 1;

        bierki.push_back(Wieza1);
        bierki.push_back(Skoczek1);
        bierki.push_back(Goniec1);
        bierki.push_back(Hetman);
        bierki.push_back(Krol);
        bierki.push_back(Goniec2);
        bierki.push_back(Skoczek2);
        bierki.push_back(Wieza2);
    }

    return bierki;
}

vector<Ruch> parse()
{
    ifstream game;
    game.open("game.txt");

    Ruch curr;

    vector<Ruch> ruchy;
    char temp;

    while (!game.eof())
    {
        game.get(temp);
        curr.x1 = temp - 'A';
        game.get(temp);
        curr.y1 = temp - '0' - 1;
        game.get(temp);
        curr.x2 = temp - 'A';
        game.get(temp);
        curr.y2 = temp - '0' - 1;
        game.get(temp); //newline
        ruchy.push_back(curr);
    }

    return ruchy;
}

vector<Wsp> calcMove(int x1, int y1, int x2, int y2)
{
    vector <Wsp> res;
    int nrOfSteps = 40;
    float newX1 = -1.1f+(2.2/7.0)*x1;
    float newY1 = 1.1f-(2.2/7.0)*y1;
    float newX2 = -1.1f+(2.2/7.0)*x2;
    float newY2 = 1.1f-(2.2/7.0)*y2;
    float lengthX = newX2-newX1;
    float lengthY = newY2-newY1;
    float stepX = lengthX/float(nrOfSteps);
    float stepY = lengthY/float(nrOfSteps);
    Wsp temp;
    float a = -16.0/(5.0 * nrOfSteps * nrOfSteps);
    for(int i = 0; i <= nrOfSteps; i++)
    {
        temp.x = newX2-i*stepX;
        temp.y = newY2-i*stepY;
        temp.z = a * float(pow(i-(nrOfSteps/2), 2)) + 0.8;
        res.push_back(temp);
    }
    return res;
}


int main(void)
{
    cout << "Wczytuje modele bierek..." << endl;
    vector<Ruch> ruchy = parse();
    vector<bierka*>  bierki = initBierki();

	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1000, 1000, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure

	float angle_x=0; //current rotation angle of the object, x axis
	float angle_y=0; //current rotation angle of the object, y axis
	float viewer_shift=0;
	glfwSetTime(0); //Zero the timer

    int i=0, time=0;
    vector<Wsp> animacja;

    //Main application loop
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
        angle_x+=speed_x*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_y+=speed_y*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		viewer_shift+=viewer_speed*glfwGetTime();
        glfwSetTime(0); //Zero the timer

        if(reset) {
            angle_x=0;
            angle_y=0;
            viewer_shift=0;
        }

        drawScene(window, angle_x, angle_y, viewer_shift, bierki); //Execute drawing procedure

        if(i < ruchy.size() && time % 80 == 0 && time > 200)
        {
            cout << "Obsluguje ruch: " <<
            (char)('A'+ruchy.at(i).x1) <<
            ruchy.at(i).y1+1 << "->" <<
            (char)('A'+ruchy.at(i).x2) <<
            ruchy.at(i).y2+1 << endl;
            for(int j=0; j < bierki.size(); j++)
            {
                if(bierki.at(j)->getX() == ruchy.at(i).x2 &&
                   bierki.at(j)->getY() == ruchy.at(i).y2 )
                {
                    animacja.clear();
                    animacja = calcMove(ruchy.at(i).x2, ruchy.at(i).y2, -10, ruchy.at(i).y2);

                    bierki.at(j)->animacja = animacja;
                    bierki.at(j)->over();
                    bierki.at(j)->moving = 1;
                    bierki.at(j)->death = 1;
                }

                if(bierki.at(j)->getX() == ruchy.at(i).x1 &&
                   bierki.at(j)->getY() == ruchy.at(i).y1 )
                {
                    animacja.clear();
                    animacja = calcMove(ruchy.at(i).x1, ruchy.at(i).y1, ruchy.at(i).x2, ruchy.at(i).y2);

                    bierki.at(j)->animacja = animacja;
                    bierki.at(j)->ruch(ruchy.at(i).x2, ruchy.at(i).y2);
                    bierki.at(j)->moving = 1;
                }
            }
            i++;
        }
        time++;

		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
