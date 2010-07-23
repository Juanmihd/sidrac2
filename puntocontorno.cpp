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

void GrahamScan(QList<PuntoContornoLight> &puntos){
    PuntoContornoLight p0;
    QMultiMap<PuntoContornoLight,PuntoContornoLight> puntosOrdenados;
    int i, posMin;
    float cat, alt;
    //Seleccionar el punto con menor altura y mas cerca del eje
    for(i=0; i<puntos.size(); ++i)
        if(puntos.at(i).altura < puntos.at(posMin).altura ||
           (puntos.at(i).altura == puntos.at(posMin).altura &&
            puntos.at(i).distancia < puntos.at(posMin).distancia))
                posMin = i;
    //se hace que ese punto este en el primer lugar
    p0.altura = puntos.at(0).altura;
    p0.distancia = puntos.at(0).distancia;
    puntos[0].altura = puntos.at(posMin).altura;
    puntos[0].distancia = puntos.at(posMin).distancia;
    puntos[posMin].altura = p0.altura;
    puntos[posMin].distancia = p0.distancia;
    //Se ordenan el resto de puntos segun el angulo
    for(i=1; i<puntos.size(); ++i){
        cat = puntos[i].distancia - p0.distancia;
        alt = puntos[i].altura - p0.altura;
        puntos[i].angulo = atan2(alt,cat);
        puntos[i].dist2 = sqrt(cat*cat + alt*alt);
        puntosOrdenados.insertMulti(puntos[i],puntos[i]);
    }
    puntos.clear();
    puntos.append(p0);
    puntos.append(puntosOrdenados.values());
    for(i=0; i<puntos.size(); ++i){

    }


}
