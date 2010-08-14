#include "puntocontorno.h"

puntoContorno::puntoContorno()
{
}

bool puntoContorno::operator< (puntoContorno p1) const{
    int numPunto2 = p1.getNumPunto();
    if(numPunto<0){
        if(numPunto2<0){
            if(altura < p1.getAltura())
                return true;
        }else{
            return true;
        }
    }else{
        if(numPunto2>0){
            if(altura > p1.getAltura())
                return true;
        }
    }
    return false;
}

bool puntoContorno::operator<=(puntoContorno p1) const{
    int numPunto2 = p1.getNumPunto();
    if(numPunto<0){
        if(numPunto2<0){
            if(altura <= p1.getAltura())
                return true;
        }else{
            return true;
        }
    }else{
        if(numPunto2>0){
            if(altura >= p1.getAltura())
                return true;
        }
    }
    return false;
}

bool puntoContorno::operator> (puntoContorno p1) const{
    int numPunto2 = p1.getNumPunto();
    if(numPunto<0){
        if(numPunto2<0){
            if(altura > p1.getAltura())
                return true;
        }
    }else{
        if(numPunto2>0){
            if(altura < p1.getAltura())
                return true;
        }else{
            return true;
        }
    }
    return false;
}

bool puntoContorno::operator>=(puntoContorno p1) const{
    int numPunto2 = p1.getNumPunto();
    if(numPunto<0){
        if(numPunto2<0){
            if(altura >= p1.getAltura())
                return true;
        }
    }else{
        if(numPunto2>0){
            if(altura <= p1.getAltura())
                return true;
        }else{
            return true;
        }
    }
    return false;
}

bool puntoContorno::operator!=(puntoContorno p1) const{
    int numPunto2 = p1.getNumPunto();
    if(numPunto<0){
        if(numPunto2<0){
            if(altura != p1.getAltura())
                return true;
        }else{
            return true;
        }
    }else{
        if(numPunto2>0){
            if(altura != p1.getAltura())
                return true;
        }else{
            return true;
        }
    }
    return false;
}

bool puntoContorno::operator==(puntoContorno p1) const{
    int numPunto2 = p1.getNumPunto();
    if(numPunto<0){
        if(numPunto2<0){
            if(altura == p1.getAltura())
                return true;
        }
    }else{
        if(numPunto2>0){
            if(altura == p1.getAltura())
                return true;
        }
    }
    return false;
}


float ccw(PuntoContornoLight p1, PuntoContornoLight p2, PuntoContornoLight p3){
    return (p2.distancia - p1.distancia)*(p3.altura - p1.altura) - (p2.altura - p1.altura)*(p3.distancia - p1.distancia);
}
