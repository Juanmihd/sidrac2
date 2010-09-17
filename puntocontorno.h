/**
  * Autor: Juan Miguel Huertas Delgado
  * puntoContorno.cpp puntoContorno.h
  *
  * Clase que contiene los puntos del contorno, con varios métodos para poder almacenarlo por orden, por altura o por distancia
  *
  *
  */


#ifndef PUNTOCONTORNO_H
#define PUNTOCONTORNO_H

#include "estructurasDeDatos.h"
#include "vcg/space/point3.h"

/**
  @brief Clase puntoContorno, es la clase elemental para los puntos que se seleccionan del contorno.
Simplemente tiene altura y distancia (al eje). Componentes x e y, realmente, que indican el contorno en un plano
  */
class puntoContorno
{
public:
    /**
      @brief Conductor por defecto de puntoContorno
      */
    puntoContorno();
    /**
      @brief Conductor a partir de la posicon de puntoContorno
      @param posicion0 : posicion tridimensional del punto
      @param numPunto0 : el numero del punto
      @param altura0 : la altura del punto
      */
    puntoContorno(vcg::Point3f posicion0, int numPunto0, float altura0);
    /** La altura del punto contorno, en funcion de la mas pequeña */
    float altura;
    /** La distancia al eje de rotacion */
    float distancia;
    /**
      @brief Devuelve la posicion del punto
      @return La posicion
      */
    vcg::Point3f getPosicion(){return posicion;}
    /**
      @brief Devuelve el numero del punto
      @return El numero del punto
      */
    int getNumPunto(){return numPunto;}
    /**
      @brief Devuelve la altura del punto
      @return La altura
      */
    float getAltura(){return altura;}
    /**
      @brief Asigna una posicion al punto
      @param posicion0 : la posicion del punto
      */
    void setPosicion(vcg::Point3f posicion0){posicion.X() = posicion0.X();posicion.Y() = posicion0.Y(); posicion.Z() = posicion0.Z();}
    /**
      @brief Asigna el numero del punto
      @param numPunto0 : el numero del punto
      */
    void setNumPunto(int numPunto0){numPunto = numPunto0;}
    /**
      @brief Asigna la altura del punto
      @param altura0 : la altura del punto
      */
    void setAltura(float altura0){altura = altura0;}
    /**
      @brief Devuelve la distancia del punto al eje de rotacion
      @return La distancia
      */
    float getDistancia(){return distancia;}
    /**
      @brief Asigna una distancia al punto
      @param distancia0 : la distancia del punto
      */
    void setDistancia(float distancia0){distancia = distancia0;}
    /**
    @brief Operator< que permite comparar dos puntos
    @param p1 : PuntoContorno con el que se compara
    @return El valor del booleano según lo especificado por esta función
      */
    bool operator< (puntoContorno p1) const;
    /**
    @brief Operator<= que permite comparar dos puntos
    @param p1 : PuntoContorno con el que se compara
    @return El valor del booleano según lo especificado por esta función
      */
    bool operator<=(puntoContorno p1) const;
    /**
    @brief Operator> que permite comparar dos puntos
    @param p1 : PuntoContorno con el que se compara
    @return El valor del booleano según lo especificado por esta función
      */
    bool operator> (puntoContorno p1) const;
    /**
    @brief Operator>= que permite comparar dos puntos
    @param p1 : PuntoContorno con el que se compara
    @return El valor del booleano según lo especificado por esta función
      */
    bool operator>=(puntoContorno p1) const;
    /**
    @brief Operator!= que permite comparar dos puntos
    @param p1 : PuntoContorno con el que se compara
    @return El valor del booleano según lo especificado por esta función
      */
    bool operator!=(puntoContorno p1) const;
    /**
    @brief Operator== que permite comparar dos puntos
    @param p1 : PuntoContorno con el que se compara
    @return El valor del booleano según lo especificado por esta función
      */
    bool operator==(puntoContorno p1) const;
private:
    vcg::Point3f posicion;
    int numPunto;
};


#endif // PUNTOCONTORNO_H
