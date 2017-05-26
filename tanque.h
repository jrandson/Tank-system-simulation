#ifndef TANQUE_H
#define TANQUE_H

class Tanque
{
public:
    Tanque();
    double getNivelTq1();
    double getNivelTq2();
    double acionaBomba(double sinal);
    void escoaTanque1(double nivelTanque, double torneira);
    void escoaTanque2(double niveltanque, double torneira);
    double getVolumeTanque1();
    double getVolumeTanque2();




private:
    double const K = 10;//cm3/v constante da bomba
    double const AreaOrificioSaida = 0.17813919765; //cm2
    double const AreaTanque = 15.95179 ;//cm2 base
    double const alturaTanque = 30; // cm

    double nivelTq1 = 0;
    double nivelTq2 = 0;
};

#endif // TANQUE_H
