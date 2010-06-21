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


