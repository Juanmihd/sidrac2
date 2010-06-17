#include "ejeplanos.h"

ejePlanos::ejePlanos(){
    numPlanos = -1;
}

void ejePlanos::setEspacio(float diagonal, int numero, vcg::Point3f direccion){
    diag = diagonal;
    numPlanos = numero;
    planoDeCorte = direccion;
    offsetInicial = -diagonal/2.;
    incrementoOffset = diagonal / numPlanos;

    angulo1 = acos(direccion.Z()/
                   (sqrt(pow(direccion.X(),2)+pow(direccion.Y(),2)+pow(direccion.Z(),2))));

    angulo2 = atan2(direccion.Y(), direccion.X());

    angulo1 = angulo1 * 180. / PI;
    angulo2 = angulo2 * 180. / PI;
}

void ejePlanos::setDireccion(vcg::Point3f direccion){
    planoDeCorte = direccion;

    angulo1 = acos(direccion.Z()/
                   (sqrt(pow(direccion.X(),2)+pow(direccion.Y(),2)+pow(direccion.Z(),2))));

    angulo2 = atan2(direccion.Y(), direccion.X());

    angulo1 = angulo1 * 180. / PI;
    angulo2 = angulo2 * 180. / PI;
}
