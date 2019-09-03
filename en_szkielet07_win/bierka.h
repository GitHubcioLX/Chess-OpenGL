#ifndef BIERKA_H
#define BIERKA_H
#include<vector>
#include<string>

using namespace std;

typedef struct wsp {
    float x;
    float y;
    float z;
} Wsp;

class bierka
{
    public:
        bierka(int, int, bool);
        copy(const bierka&);
        virtual ~bierka();
        void ruch(int, int);
        void over();
        int getX();
        int getY();
        void setX(int);
        void setY(int);
        bool team;
        vector < float > wierzcholki;
        vector < float > tekstury;
        vector < float > normalne;

        vector < float > vert;
        vector < float > tex;
        vector < float > norm;

        vector < int > w_indeksy;
        vector < int > t_indeksy;
        vector < int > n_indeksy;

        vector < Wsp > animacja;

        vector <string> split(string);
        string simplify(string);
        int str2int(string);
        float str2f(string);
        void modelLoader(string);
        int licz_wierz;
        bool moving;

    protected:
        int pozycjaX;
        int pozycjaY;

    private:
};

#endif // BIERKA_H
