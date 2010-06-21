#ifndef VOXEL_H
#define VOXEL_H

#include "vcg/space/point3.h"

class Voxel
{
public:
    Voxel();
    Voxel(int x, int y, int z);
    Voxel(vcg::Point3i nVoxel);
    const int contador(){return Contador;}
    void asignarContador(int contador){Contador = contador;}
    void incrementar(){++Contador;}
    bool operator< (Voxel p1) const;
    bool operator<=(Voxel p1) const;
    bool operator> (Voxel p1) const;
    bool operator>=(Voxel p1) const;
    bool operator!=(Voxel p1) const;
    bool operator==(Voxel p1) const;
    const int X(){return posicion.X();}
    const int Y(){return posicion.Y();}
    const int Z(){return posicion.Z();}
    vcg::Point3i posicion;
    void X(int x){posicion = vcg::Point3i(x,posicion.Y(),posicion.Z());}
    void Y(int y){posicion = vcg::Point3i(posicion.X(),y,posicion.Z());}
    void Z(int z){posicion = vcg::Point3i(posicion.X(),posicion.Y(),z);}

private:
    int Contador;
};

class PuntoOrdenable
{
public:
    PuntoOrdenable();
    PuntoOrdenable(float x, float y, float z);
    PuntoOrdenable(vcg::Point3f nVoxel);
    float angulo(){return Angulo;}
    void asignarAngulo(float angulo){Angulo = angulo;}
    float distancia(){return Distancia;}
    void asignarDistancia(float distancia){Distancia = distancia;}
    void set(vcg::Point3f valor){posicion = vcg::Point3f(valor);}
    bool operator< (PuntoOrdenable p1) const;
    bool operator<=(PuntoOrdenable p1) const;
    bool operator> (PuntoOrdenable p1) const;
    bool operator>=(PuntoOrdenable p1) const;
    bool operator!=(PuntoOrdenable p1) const;
    bool operator==(PuntoOrdenable p1) const;
    const float X(){return posicion.X();}
    const float Y(){return posicion.Y();}
    const float Z(){return posicion.Z();}
    vcg::Point3f posicion;
    void X(float x){posicion = vcg::Point3f(x,posicion.Y(),posicion.Z());}
    void Y(float y){posicion = vcg::Point3f(posicion.X(),y,posicion.Z());}
    void Z(float z){posicion = vcg::Point3f(posicion.X(),posicion.Y(),z);}

private:
    float Angulo;
    float Distancia;
};

#endif // VOXEL_H
