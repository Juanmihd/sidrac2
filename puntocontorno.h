#ifndef PUNTOCONTORNO_H
#define PUNTOCONTORNO_H

#include "vcg/space/point3.h"


class puntoContorno
{
public:
    puntoContorno();
    puntoContorno(vcg::Point3f posicion0, int numPunto0, float altura0);
    vcg::Point3f getPosicion(){return posicion;}
    int getNumPunto(){return numPunto;}
    float getAltura(){return altura;}
    void setPosicion(vcg::Point3f posicion0){posicion.X() = posicion0.X();posicion.Y() = posicion0.Y(); posicion.Z() = posicion0.Z();}
    void setNumPunto(int numPunto0){numPunto = numPunto0;}
    void setAltura(float altura0){altura = altura0;}
    float getDistancia(){return distancia;}
    void setDistancia(float distancia0){distancia = distancia0;}
    bool operator< (puntoContorno p1) const;
    bool operator<=(puntoContorno p1) const;
    bool operator> (puntoContorno p1) const;
    bool operator>=(puntoContorno p1) const;
    bool operator!=(puntoContorno p1) const;
    bool operator==(puntoContorno p1) const;
private:
    vcg::Point3f posicion;
    int numPunto;
    float altura;
    float distancia;
};



#endif // PUNTOCONTORNO_H
