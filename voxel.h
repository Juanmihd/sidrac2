/**
  * Autor: Juan Miguel Huertas Delgado
  * voxel.cpp voxel.h
  *
  * Este fichero contiene dos clases, Voxel y PuntoOrdenable. Ambas se usan para el cálculo del eje
  *
  *
  */


#ifndef VOXEL_H
#define VOXEL_H

#include "vcg/space/point3.h"

/**
@brief Voxel es una de las clases fundamentales de este proyecto. Representa y cuenta el numero de intersecciones
que hay en un mismo voxel. Tiene implementadas las funciones típicas de ordenamiento
*/
class Voxel
{
public:
    /**
    @brief Constructor por defecto de la clase Voxel
    */
    Voxel();
    /**
    @brief Constructor a traves de valores de la clase Voxel
    @param x : La posicion x (int)
    @param y : La posicion y (int)
    @param z : La posicion z (int)
    */
    Voxel(int x, int y, int z);
    /**
    @brief Constructor de la clase Voxel
    @param nVoxel : el punto en 3D (int)
    */
    Voxel(vcg::Point3i nVoxel);
    /**
    @brief Devuelve el valor del contador de intersecciones de este voxel
    @return el valor del contador
    */
    const int contador(){return Contador;}
    /**
    @brief asigna un valor al contador
    @param contador : El valor del contador
    */
    void asignarContador(int contador){Contador = contador;}
    /**
    @brief Incrementa en uno el valor del contador
    */
    void incrementar(){++Contador;}
    /**
    @brief Operator< que permite comparar dos voxels
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator< (Voxel p1) const;
    /**
    @brief Operator<= que permite comparar dos voxels
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator<=(Voxel p1) const;
    /**
    @brief Operator> que permite comparar dos voxels
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator> (Voxel p1) const;
    /**
    @brief Operator>= que permite comparar dos voxels
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator>=(Voxel p1) const;
    /**
    @brief Operator!= que permite comparar dos voxels
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator!=(Voxel p1) const;
    /**
    @brief Operator== que permite comparar dos voxels
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator==(Voxel p1) const;
    /**
    @brief Devuelve el valor de X del Voxel
    @return El valor X
    */
    const int X(){return posicion.X();}
    /**
    @brief Devuelve el valor de Y del Voxel
    @return El valor Y
    */
    const int Y(){return posicion.Y();}
    /**
    @brief Devuelve el valor de Z del Voxel
    @return El valor Z
    */
    const int Z(){return posicion.Z();}
    /**
      @brief La posicion se almacena de forma publica, para reducir el coste de las funciones
      */
    vcg::Point3i posicion;
    /**
    @brief Asigna un nuevo valor de la posición de x
    @param x : Nuevo valor de x
    */
    void X(int x){posicion = vcg::Point3i(x,posicion.Y(),posicion.Z());}
    /**
    @brief Asigna un nuevo valor de la posición de y
    @param y : Nuevo valor de y
    */
    void Y(int y){posicion = vcg::Point3i(posicion.X(),y,posicion.Z());}
    /**
    @brief Asigna un nuevo valor de la posición de z
    @param z : Nuevo valor de z
    */
    void Z(int z){posicion = vcg::Point3i(posicion.X(),posicion.Y(),z);}

private:
    int Contador;
};

/**
@brief PuntoOrdenable es una clase que almacena un punto, y que tiene como principal caracteristica que tiene implementadas
las funciones necesarias para poder ser utilizado en un QMap.
*/
class PuntoOrdenable
{
public:
    /**
    @brief Constructor por defecto de la clase PuntoOrdenable
    */
    PuntoOrdenable();
    /**
    @brief Constructor de la clase PuntoOrdenable
    @param x : Valor X del punto
    @param y : Valor Y del punto
    @param z : Valor Z del punto
    */
    PuntoOrdenable(float x, float y, float z);
    /**
    @brief Constructor de copias de la clase PuntoOrdenable
    @param nVoxel : Es el punto anterior
    */
    PuntoOrdenable(vcg::Point3f nVoxel);
    /**
    @brief Obtiene el ángulo del punto
    @return El angulo del punto
    */
    float angulo(){return Angulo;}
    /**
    @brief Asigna el valor del ánulo
    @param angulo : El nuevo valor
    */
    void asignarAngulo(float angulo){Angulo = angulo;}
    /**
    @brief Obtiene la distancia del punto
    @return La distancia del munto
    */
    float distancia(){return Distancia;}
    /**
    @brief Asigna el valor de distancia
    @param distancia : la nueva distancia
    */
    void asignarDistancia(float distancia){Distancia = distancia;}
    /**
    @brief Asigna el punto desde otro punto
    @param valor : el punto
    */
    void set(vcg::Point3f valor){posicion = vcg::Point3f(valor);}
    /**
    @brief Operator< que permite comparar dos puntosordenables
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator< (PuntoOrdenable p1) const;
    /**
    @brief Operator<= que permite comparar dos puntosordenables
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator<=(PuntoOrdenable p1) const;
    /**
    @brief Operator> que permite comparar dos puntosordenables
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator> (PuntoOrdenable p1) const;
    /**
    @brief Operator>= que permite comparar dos puntosordenables
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator>=(PuntoOrdenable p1) const;
    /**
    @brief Operator!= que permite comparar dos puntosordenables
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator!=(PuntoOrdenable p1) const;
    /**
    @brief Operator== que permite comparar dos puntosordenables
    @param p1 : el nuevo punto con el que se compara
    @return Devuelve el valor booleano segun el operador
    */
    bool operator==(PuntoOrdenable p1) const;
    /**
    @brief Devuelve el valor de X
    @return El valor de X
    */
    const float X(){return posicion.X();}
    /**
    @brief Devuelve el valor de Y
    @return El valor de Y
    */
    const float Y(){return posicion.Y();}
    /**
    @brief Devuelve el valor de Z
    @return El valor de Z
    */
    const float Z(){return posicion.Z();}
    /**
      @brief La posicion se almacena de forma publica, para reducir el coste de las funciones
      */
    vcg::Point3f posicion;
    /**
    @brief Asigna el valor de X
    @param x : El nuevo X
    */
    void X(float x){posicion = vcg::Point3f(x,posicion.Y(),posicion.Z());}
    /**
    @brief Asigna el valor de Y
    @param y : El nuevo Y
    */
    void Y(float y){posicion = vcg::Point3f(posicion.X(),y,posicion.Z());}
    /**
    @brief Asigna el valor de Z
    @param z : El nuevo Z
    */
    void Z(float z){posicion = vcg::Point3f(posicion.X(),posicion.Y(),z);}

private:
    float Angulo;
    float Distancia;
};

#endif // VOXEL_H
