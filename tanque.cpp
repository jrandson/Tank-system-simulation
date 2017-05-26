#include "tanque.h"
#include "qmath.h"

Tanque::Tanque()
{
    this->nivelTq1 = 0;
    this->nivelTq2 = 0;
}



double Tanque::acionaBomba(double sinal){

    if(sinal > 4.0) sinal = 4.0;
    if(sinal < -4.0) sinal = -4.0;

    this->nivelTq1 += 0.1*(sinal * this->K) / this->AreaTanque;

    return sinal;
}


double Tanque::getVolumeTanque1()
{
    return this->AreaTanque * this->getNivelTq1();
}

double Tanque::getVolumeTanque2()
{
    return this->AreaTanque * this->getNivelTq2();
}

void Tanque::escoaTanque1(double nivelTanque, double torneira)
{

    double deltaNivel = torneira * 0.1*this->AreaOrificioSaida * sqrt(2*980*nivelTanque)/this->AreaTanque;

    this->nivelTq1 -= deltaNivel;

    if(this->nivelTq1 < 0) this->nivelTq1 = 0;

    this->nivelTq2 += deltaNivel;

}

void Tanque::escoaTanque2(double niveltanque, double torneira)
{

    double deltaNivel = torneira * 0.1*this->AreaOrificioSaida * sqrt(2*980*niveltanque)/this->AreaTanque;
    this->nivelTq2 -= deltaNivel;
    if(this->nivelTq2 < 0) this->nivelTq2 = 0;
}

double Tanque::getNivelTq1(){
    return this->nivelTq1;
}

double Tanque::getNivelTq2()
{
    return this->nivelTq2;
}
