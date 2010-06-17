#ifndef EJEPLANOS_H
#define EJEPLANOS_H

#include "vcg/simplex/vertex/base.h"
#include "vcg/simplex/face/base.h"
#include "vcg/complex/trimesh/base.h"
#ifndef PI
#define PI 3.141592
#endif

class ejePlanos
{
public:
    ejePlanos();
    void setEspacio(float diagonal, int numero, vcg::Point3f direccion);
    void setDireccion(vcg::Point3f direccion);
    float getOffset(){return offsetInicial;}
    float getIncremento(){return incrementoOffset;}
    float getAngulo1(){return angulo1;}
    float getAngulo2(){return angulo2;}
    int getNumero(){return numPlanos;}
    vcg::Point3f direccion(){return planoDeCorte;}
    void dibujarPlanos();

private:
    vcg::Point3f planoDeCorte;
    int numPlanos;
    float diag;
    float offsetInicial;
    float incrementoOffset;
    float angulo1;
    float angulo2;
};

#endif // EJEPLANOS_H
