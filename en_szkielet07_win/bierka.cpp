#include "bierka.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<cstdlib>

using namespace std;

bierka::bierka(int x, int y, bool t)
{
    pozycjaX = x;
    pozycjaY = y;
    team = t;
    licz_wierz = 0;
}

bierka::copy(const bierka &og)
{
    wierzcholki = og.wierzcholki;
    tekstury = og.tekstury;
    normalne = og.normalne;
    vert = og.vert;
    tex = og.tex;
    norm = og.norm;
    w_indeksy = og.w_indeksy;
    t_indeksy = og.t_indeksy;
    n_indeksy = og.n_indeksy;
    licz_wierz = og.licz_wierz;
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

int licz_wierz;

vector <float> wierzcholki;
vector <float> tekstury;
vector <float> normalne;

vector <int> w_indeksy;
vector <int> t_indeksy;
vector <int> n_indeksy;

vector <float> vert;
vector <float> tex;
vector <float> norm;

vector <string> bierka::split(string wiersz)
    {
        vector <string> res;
        string pom = "";
        for(int i = 0; i < wiersz.length(); i++)
        {
            if(wiersz[i] == ' ')
            {
                res.push_back(pom);
                pom = "";
            }
            else pom += wiersz[i];
        }
        res.push_back(pom);
        return res;
    }

string bierka::simplify(string wiersz)
    {
        string res = wiersz.substr(2);
        for(int i = 0; i < res.length(); i++)
        {
            if(res[i] == '/') res[i] = ' ';
        }
        return res;
    }

void bierka::modelLoader(string filename) {
    string wiersz, koord;
    vector <string> splitted;
    ifstream plik;
    plik.open(filename.c_str());
    while( getline(plik, wiersz) )
    {
        splitted = split(wiersz);
        if(wiersz[0] == 'v' && wiersz[1] == ' ')
        {
            for(int i = 1; i < splitted.size(); i++)
            {
                koord = splitted[i];
                wierzcholki.push_back(atof(koord.c_str())/240.0);
            }
        }
        else if(wiersz[0] == 'v' && wiersz[1] == 't')
        {
            for(int i = 1; i < splitted.size(); i++)
            {
                koord = splitted[i];
                tekstury.push_back(atof(koord.c_str())/240.0);
            }
        }
        else if(wiersz[0] == 'v' && wiersz[1] == 'n')
        {
            for(int i = 1; i < splitted.size(); i++)
            {
                koord = splitted[i];
                normalne.push_back(atof(koord.c_str())/240.0);
            }
        }
        else if(wiersz[0] == 'f')
        {
            string uproszczony = simplify(wiersz);
            string pom = "";
            int iter = 0;
            for(int i = 0; i < uproszczony.length(); i++)
            {
                if(uproszczony[i] == ' ')
                {
                    switch(iter)
                    {
                    case 0:
                        w_indeksy.push_back(atoi(pom.c_str()) - 1);
                        iter ++;
                        break;

                    case 1:
                        if(pom != "") t_indeksy.push_back(atoi(pom.c_str()) - 1);
                        iter ++;
                        break;

                    case 2:
                        n_indeksy.push_back(atoi(pom.c_str()) - 1);
                        iter = 0;
                        break;

                    default:
                        break;
                    }
                    pom = "";
                }
                else pom = pom + uproszczony[i];
            }
            if(pom != "") n_indeksy.push_back(atoi(pom.c_str()) - 1);
        }
    }

    for(int i = 0; i < w_indeksy.size(); i++)
    {
        for(int j = 0; j < 3; j++)
        {
            vert.push_back(wierzcholki[3*w_indeksy[i] + j]);
            norm.push_back(normalne[3*n_indeksy[i] + j]);
        }
        if(tekstury.size() > 0)
        {
            for(int j = 0; j < 2; j++)
            {
                tex.push_back(tekstury[2*t_indeksy[i] + j]);
            }
        }
    }
    licz_wierz = w_indeksy.size();
    cout << licz_wierz << endl;
    plik.close();
}
