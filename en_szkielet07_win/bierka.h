#ifndef BIERKA_H
#define BIERKA_H
#include <vector>
#include<string>

using namespace std;


class bierka
{
    public:
        bierka(int, int);
        virtual ~bierka();
        void ruch(int, int);
        void over();
        int getX();
        int getY();
        void setX(int);
        void setY(int);
        vector < float > vertices;
        vector < float > textures;
        vector < float > normals;

        vector < float > vertices3;
        vector < float > textures3;
        vector < float > normals3;

        vector < float > vertices4;
        vector < float > textures4;
        vector < float > normals4;

        vector < int > index_vert_troj;
        vector < int > index_tex_troj;
        vector < int > index_norm_troj;

        vector < int > index_vert_czworo;
        vector < int > index_tex_czworo;
        vector < int > index_norm_czworo;

        vector <string> split_line(string);
        int wyst(string);
        string f_line(string);
        int str2int(string);
        float str2f(string);
        void ObjLoader(string);


    protected:
        int pozycjaX;
        int pozycjaY;

    private:
};

#endif // BIERKA_H
