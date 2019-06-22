#include "bierka.h"
#include<iostream>
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
