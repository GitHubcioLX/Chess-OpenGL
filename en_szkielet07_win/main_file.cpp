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
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"


float speed_x=0; //angular speed in radians
float speed_y=0; //angular speed in radians
float aspectRatio=1;
ShaderProgram *sp; //Pointer to the shader program

GLuint tex0;
GLuint tex1;

//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if (action==GLFW_PRESS) {
        if (key==GLFW_KEY_LEFT) speed_x=-PI/2;
        if (key==GLFW_KEY_RIGHT) speed_x=PI/2;
        if (key==GLFW_KEY_UP) speed_y=PI/2;
        if (key==GLFW_KEY_DOWN) speed_y=-PI/2;
    }
    if (action==GLFW_RELEASE) {
        if (key==GLFW_KEY_LEFT) speed_x=0;
        if (key==GLFW_KEY_RIGHT) speed_x=0;
        if (key==GLFW_KEY_UP) speed_y=0;
        if (key==GLFW_KEY_DOWN) speed_y=0;
    }
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

class Model{
    public:
        std::vector < float > vertices;
        std::vector < float > textures;
        std::vector < float > normals;

        std::vector < float > vertices3;
        std::vector < float > textures3;
        std::vector < float > normals3;

        std::vector < float > vertices4;
        std::vector < float > textures4;
        std::vector < float > normals4;

        std::vector < int > index_vert_troj;
        std::vector < int > index_tex_troj;
        std::vector < int > index_norm_troj;

        std::vector < int > index_vert_czworo;
        std::vector < int > index_tex_czworo;
        std::vector < int > index_norm_czworo;

        std::vector <string> split_line(string line)
        {
            std::vector < string > splitted;
            string pom = "";
            for(int i = 0; i < line.length(); i++)
            {
                if(line[i] == ' ')
                {
                    splitted.push_back(pom);
                    pom = "";
                }
                else
                {
                    pom += line[i];
                }
            }
            splitted.push_back(pom);
            return splitted;
        }

        int wyst(string line)
        {
            int licznik = 0;
            for(int i =0; i<line.length();i++)
            {
                if(line[i] == '/')
                {
                    licznik ++;
                }
            }
            return licznik;
        }

        string f_line(string line)
        {
            string newline = line.substr(2);
            for(int i = 0; i<newline.length(); i++)
            {
                if(newline[i] == ' ')
                {
                    newline[i] = '/';
                }
            }
            return newline;
        }

        int str2int(string str)
        {
            int integer = 0;
            for(int i = 0; i < str.size(); i++)
            {
                integer = integer * 10 + int(str[i] - '0');
            }
            return integer;
        }

        float str2f(string str)
        {
            float d, integer = 0, decimal = 0, k = 0.1;
            int sign, flag = 0;
            if(str[0] == '-'){sign = -1;}
            else{sign = 1;}
            for(int i = 0; i < str.length(); i++)
            {
                if(str[i] >= '0' && str[i] <= '9')
                {
                    if(flag == 0)
                    {
                        integer *= 10;
                        integer += int(str[i] - '0');
                    }
                    else
                    {
                        decimal += k * int(str[i] - '0');
                        k /= 10;
                    }
                }
                else if(str[i] == '.'){flag = 1;}
            }
            d = sign * (integer + decimal);
            return d;
        }

    void ObjLoader(string file) {
      string line, splitted;
      ifstream myfile;
      myfile.open(file.c_str());
      while ( getline (myfile,line) )
      {
        if(line[0] == 'v' && line[1] == ' ')
        {
            if(line[2] == ' ')
            {
                line.erase(1,1);
            }
            for(int i = 1; i < split_line(line).size(); i++)
            {
                splitted = split_line(line)[i];
                this -> vertices.push_back(str2f(splitted)/50.0);
            }
        }
        else if(line[0] == 'v' && line[1] == 't')
        {
            for(int i = 1; i < split_line(line).size(); i++)
            {
                splitted = split_line(line)[i];
                this -> textures.push_back(str2f(splitted)/50.0);
            }
        }
        else if(line[0] == 'v' && line[1] == 'n')
        {
            for(int i = 1; i < split_line(line).size(); i++)
            {
                splitted = split_line(line)[i];
                this -> normals.push_back(str2f(splitted)/50.0);
            }
        }
        else if(line[0] == 'f')
        {
            string newline = f_line(line);
            int licznik = 0;
            string pom = "";
            int n = wyst(newline);
            for(int i =0; i<newline.length(); i++)
            {
                if(newline[i] == '/')
                {
                    if(licznik == 0)
                    {
                        if(n > 9)
                        {
                            this -> index_vert_czworo.push_back(str2int(pom) - 1);
                        }
                        else
                        {
                            this -> index_vert_troj.push_back(str2int(pom) - 1);
                        }
                    }
                    if(licznik == 1)
                    {
                        if(pom != ""){
                            if(n > 9)
                            {
                                this -> index_tex_czworo.push_back(str2int(pom) - 1);
                            }
                            else
                            {
                                this -> index_tex_troj.push_back(str2int(pom) - 1);
                            }
                        }
                    }
                    if(licznik == 2)
                    {
                        if(n > 9)
                        {
                            this -> index_norm_czworo.push_back(str2int(pom) - 1);
                        }
                        else
                        {
                            this -> index_norm_troj.push_back(str2int(pom) - 1);
                        }
                    }
                    if(licznik == 2)
                    {
                        licznik = 0;
                    }
                    else{
                        licznik ++;
                    }
                    pom = "";
                }
                else
                {
                    pom = pom + newline[i];
                }
            }
            if(n > 9)
            {
                this -> index_norm_czworo.push_back(str2int(pom) - 1);
            }
            else
            {
                this -> index_norm_troj.push_back(str2int(pom) - 1);
            }
        }
      }
      for(int i = 0; i < this -> index_vert_troj.size(); i++)
      {
          for(int j = 0; j < 3; j++)
          {
              this -> vertices3.push_back(this -> vertices[3*this -> index_vert_troj[i] + j]);
              this -> normals3.push_back(this -> normals[3*this -> index_norm_troj[i] + j]);
          }
          if(textures.size() > 0)
          {
              for(int j = 0; j < 2; j++)
              {
                  this -> textures3.push_back(this -> textures[2*this -> index_tex_troj[i] + j]);
              }
          }
      }
      for(int i = 0; i < this -> index_vert_czworo.size(); i++)
      {
          for(int j = 0; j < 3; j++)
          {
              this -> vertices4.push_back(this -> vertices[3*this -> index_vert_czworo[i] + j]);
              this -> normals4.push_back(this -> normals[3*this -> index_norm_czworo[i] + j]);
          }
          if(textures.size() > 0)
          {
              for(int j = 0; j < 2; j++)
              {
                  this -> textures4.push_back(this -> textures[2*this -> index_tex_czworo[i] + j]);
              }
          }
      }
      myfile.close();
    }
};




Model piiionek;





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
	//************Place any code here that needs to be executed once, at the program start************
	glClearColor(0,0,0,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);
	sp=new ShaderProgram("vertex.glsl",NULL,"fragment.glsl");

    tex0=readTexture("metal.png");
    tex1=readTexture("sky.png");
    piiionek.ObjLoader("pion.obj");

}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Place any code here that needs to be executed once, after the main loop ends************

    glDeleteTextures(1,&tex0);
    glDeleteTextures(1,&tex1);

    delete sp;
}

//Drawing procedure
void drawScene(GLFWwindow* window,float angle_x,float angle_y) {
	//************Place any code here that draws something inside the window******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V=glm::lookAt(
        glm::vec3(0.0f,0.0f,-5.0f),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)); //compute view matrix
    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 1.0f, 50.0f); //compute projection matrix
    glm::mat4 M=glm::mat4(1.0f);
	M=glm::rotate(M,angle_y,glm::vec3(1.0f,0.0f,0.0f)); //Compute model matrix
	M=glm::rotate(M,angle_x,glm::vec3(0.0f,1.0f,0.0f)); //Compute model matrix

    //Cube
	/*float *verts=myCubeVertices;
	float *normals=myCubeNormals;
	float *texCoords=myCubeTexCoords;
	unsigned int vertexCount=myCubeVertexCount;*/

	//Teapot
	float *verts=&piiionek.vertices3[0];
	float *normals=&piiionek.normals3[0];
	float *texCoords=&piiionek.textures3[0];
	unsigned int vertexCount=66054;

    sp->use();//activate shading program
    //Send parameters to graphics card
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));
    //glUniform4f(sp->u("lp"),0,0,-6,1); //Light coordinates in the world space

    //glUniform1i(sp->u("textureMap0"),0);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D,tex0);

    //glUniform1i(sp->u("textureMap1"),1);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D,tex1);

    glEnableVertexAttribArray(sp->a("vertex")); //Enable sending data to the attribute vertex
    glVertexAttribPointer(sp->a("vertex"),3,GL_FLOAT,false,0,verts); //Specify source of the data for the attribute vertex

    glEnableVertexAttribArray(sp->a("normal")); //Enable sending data to the attribute normal
    glVertexAttribPointer(sp->a("normal"),3,GL_FLOAT,false,0,normals); //Specify source of the data for the attribute normal

    //glEnableVertexAttribArray(sp->a("texCoord0")); //Enable sending data to the attribute color
    //glVertexAttribPointer(sp->a("texCoord0"),2,GL_FLOAT,false,0,texCoords); //Specify source of the data for the attribute color


    glDrawArrays(GL_TRIANGLES,0,vertexCount); //Draw the object

    glDisableVertexAttribArray(sp->a("vertex")); //Disable sending data to the attribute vertex
    glDisableVertexAttribArray(sp->a("normal")); //Disable sending data to the attribute normal
    //glDisableVertexAttribArray(sp->a("texCoord0")); //Disable sending data to the attribute color

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

    vector<bierka*>  bierki;

    bierka * bialaWieza1 = new bierka(0, 0);
    bierka * bialySkoczek1 = new bierka(1, 0);
    bierka * bialyGoniec1 = new bierka(2, 0);
    bierka * bialyHetman = new bierka(3, 0);
    bierka * bialyKrol = new bierka(4, 0);
    bierka * bialyGoniec2 = new bierka(5, 0);
    bierka * bialySkoczek2 = new bierka(6, 0);
    bierka * bialaWieza2 = new bierka(7, 0);

    bierki.push_back(bialaWieza1);
    bierki.push_back(bialySkoczek1);
    bierki.push_back(bialyGoniec1);
    bierki.push_back(bialyHetman);
    bierki.push_back(bialyKrol);
    bierki.push_back(bialyGoniec2);
    bierki.push_back(bialySkoczek2);
    bierki.push_back(bialaWieza2);

    bierka * bialyPion1 = new bierka(0, 1);
    bierka * bialyPion2 = new bierka(1, 1);
    bierka * bialyPion3 = new bierka(2, 1);
    bierka * bialyPion4 = new bierka(3, 1);
    bierka * bialyPion5 = new bierka(4, 1);
    bierka * bialyPion6 = new bierka(5, 1);
    bierka * bialyPion7 = new bierka(6, 1);
    bierka * bialyPion8 = new bierka(7, 1);

    bierki.push_back(bialyPion1);
    bierki.push_back(bialyPion2);
    bierki.push_back(bialyPion3);
    bierki.push_back(bialyPion4);
    bierki.push_back(bialyPion5);
    bierki.push_back(bialyPion6);
    bierki.push_back(bialyPion7);
    bierki.push_back(bialyPion8);

    bierka * czarnaWieza1 = new bierka(0, 7);
    bierka * czarnySkoczek1 = new bierka(1, 7);
    bierka * czarnyGoniec1 = new bierka(2, 7);
    bierka * czarnyHetman = new bierka(3, 7);
    bierka * czarnyKrol = new bierka(4, 7);
    bierka * czarnyGoniec2 = new bierka(5, 7);
    bierka * czarnySkoczek2 = new bierka(6, 7);
    bierka * czarnyWieza2 = new bierka(7, 7);

    bierki.push_back(czarnaWieza1);
    bierki.push_back(czarnySkoczek1);
    bierki.push_back(czarnyGoniec1);
    bierki.push_back(czarnyHetman);
    bierki.push_back(czarnyKrol);
    bierki.push_back(czarnyGoniec2);
    bierki.push_back(czarnySkoczek2);
    bierki.push_back(czarnyWieza2);

    bierka * czarnyPion1 = new bierka(0, 6);
    bierka * czarnyPion2 = new bierka(1, 6);
    bierka * czarnyPion3 = new bierka(2, 6);
    bierka * czarnyPion4 = new bierka(3, 6);
    bierka * czarnyPion5 = new bierka(4, 6);
    bierka * czarnyPion6 = new bierka(5, 6);
    bierka * czarnyPion7 = new bierka(6, 6);
    bierka * czarnyPion8 = new bierka(7, 6);

    bierki.push_back(czarnyPion1);
    bierki.push_back(czarnyPion2);
    bierki.push_back(czarnyPion3);
    bierki.push_back(czarnyPion4);
    bierki.push_back(czarnyPion5);
    bierki.push_back(czarnyPion6);
    bierki.push_back(czarnyPion7);
    bierki.push_back(czarnyPion8);

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


int main(void)
{
    vector<Ruch> ruchy = parse();
    vector<bierka*>  bierki = initBierki();

    for(int i=0; i < ruchy.size(); i++)
    {
            cout << "Obsluguje ruch " <<
            ruchy.at(i).x1 << ' ' <<
            ruchy.at(i).y1 << ' ' <<
            ruchy.at(i).x2 << ' ' <<
            ruchy.at(i).y2 << endl;
            for(int j=0; j < bierki.size(); j++)
            {
                if(bierki.at(j)->getX() == ruchy.at(i).x2 &&
                   bierki.at(j)->getY() == ruchy.at(i).y2 )
                {
                    bierki.at(j)->over();
                    bierki.erase(bierki.begin() + j);
                }

                if(bierki.at(j)->getX() == ruchy.at(i).x1 &&
                   bierki.at(j)->getY() == ruchy.at(i).y1 )
                {
                    bierki.at(j)->ruch(ruchy.at(i).x2, ruchy.at(i).y2);
                }
            }
    }



	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

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
	glfwSetTime(0); //Zero the timer
	//Main application loop
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
        angle_x+=speed_x*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
		angle_y+=speed_y*glfwGetTime(); //Add angle by which the object was rotated in the previous iteration
        glfwSetTime(0); //Zero the timer
		drawScene(window,angle_x,angle_y); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
