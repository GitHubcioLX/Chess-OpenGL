#include "bierka.h"
#include<iostream>
#include<fstream>
using namespace std;

bierka::bierka(int x, int y)
{
    pozycjaX = x;
    pozycjaY = y;
}

bierka::~bierka()
{
    //dtor
}

void bierka::ruch(int x, int y)
{
    pozycjaX = x;
    pozycjaY = y;
}

void bierka::over()
{
    pozycjaX = -1;
    pozycjaY = -1;
}

int bierka::getX()
{
    return pozycjaX;
}

int bierka::getY()
{
    return pozycjaY;
}

void bierka::setX(int x)
{
    pozycjaX = x;
}

void bierka::setY(int y)
{
    pozycjaY = y;
}

vector <string> bierka::split_line(string line)
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

int bierka::wyst(string line)
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

string bierka::f_line(string line)
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

int bierka::str2int(string str)
        {
            int integer = 0;
            for(int i = 0; i < str.size(); i++)
            {
                integer = integer * 10 + int(str[i] - '0');
            }
            return integer;
        }

float bierka::str2f(string str)
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

void bierka::ObjLoader(string file) {
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
