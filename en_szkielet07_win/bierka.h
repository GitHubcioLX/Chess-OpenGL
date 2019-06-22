#ifndef BIERKA_H
#define BIERKA_H

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

    protected:
        int pozycjaX;
        int pozycjaY;

    private:
};

#endif // BIERKA_H
