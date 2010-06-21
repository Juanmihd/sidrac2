#include "voxel.h"

Voxel::Voxel()
{
    posicion = vcg::Point3i(0,0,0);
    Contador = 1;
}

Voxel::Voxel(vcg::Point3i nVoxel){
    posicion = vcg::Point3i(nVoxel);
    Contador = 1;
}

Voxel::Voxel(int x, int y, int z){
    posicion = vcg::Point3i(x,y,z);
    Contador = 1;
}


bool Voxel::operator< (Voxel p1) const{
    if(posicion.X()<p1.X() || posicion.Y()<p1.Y() || posicion.Z()<p1.Z())
        return true;
    return false;
}

bool Voxel::operator<=(Voxel p1) const{
    if(posicion.X()<=p1.X() || posicion.Y()<=p1.Y() || posicion.Z()<=p1.Z())
        return true;
    return false;
}

bool Voxel::operator> (Voxel p1) const{
    if(posicion.X()> p1.X() || posicion.Y()> p1.Y() || posicion.Z()> p1.Z())
        return true;
    return false;
}

bool Voxel::operator>=(Voxel p1) const{
    if(posicion.X()>=p1.X() || posicion.Y()>=p1.Y() || posicion.Z()>=p1.Z())
        return true;
    return false;
}

bool Voxel::operator!=(Voxel p1) const{
    if(posicion.X()!=p1.X() || posicion.Y()!=p1.Y() || posicion.Z()!=p1.Z())
        return true;
    return false;
}

bool Voxel::operator==(Voxel p1) const{
    if(posicion.X()==p1.X() || posicion.Y()==p1.Y() || posicion.Z()==p1.Z())
        return true;
    return false;
}

PuntoOrdenable::PuntoOrdenable()
{
    posicion = vcg::Point3f(0,0,0);
}

PuntoOrdenable::PuntoOrdenable(vcg::Point3f nVoxel){
    posicion = vcg::Point3f(nVoxel);
}

PuntoOrdenable::PuntoOrdenable(float x, float y, float z){
    posicion = vcg::Point3f(x,y,z);
}


bool PuntoOrdenable::operator< (PuntoOrdenable p1) const{
    if(posicion.X()<p1.X() ||
       (posicion.X() == p1.X() && posicion.Y()<p1.Y()) ||
       (posicion.X() == p1.X() && posicion.Y() == p1.Y() && posicion.Z()<p1.Z()))
        return true;
    return false;
}

bool PuntoOrdenable::operator<=(PuntoOrdenable p1) const{
    if(posicion.X()<=p1.X() ||
       (posicion.X() == p1.X() && posicion.Y()<=p1.Y()) ||
       (posicion.X() == p1.X() && posicion.Y() == p1.Y() && posicion.Z()<=p1.Z()))
        return true;
    return false;
}

bool PuntoOrdenable::operator> (PuntoOrdenable p1) const{
    if(posicion.X()> p1.X() ||
       (posicion.X() == p1.X() && posicion.Y()> p1.Y()) ||
       (posicion.X() == p1.X() && posicion.Y() == p1.Y() && posicion.Z()> p1.Z()))
        return true;
    return false;
}

bool PuntoOrdenable::operator>=(PuntoOrdenable p1) const{
    if(posicion.X()>=p1.X() ||
       (posicion.X() == p1.X() && posicion.Y()>=p1.Y()) ||
       (posicion.X() == p1.X() && posicion.Y() == p1.Y() && posicion.Z()>=p1.Z()))
        return true;
    return false;
}

bool PuntoOrdenable::operator!=(PuntoOrdenable p1) const{
    if(posicion.X()!=p1.X() ||
       (posicion.X() == p1.X() && posicion.Y()!=p1.Y()) ||
       (posicion.X() == p1.X() && posicion.Y() == p1.Y() && posicion.Z()!=p1.Z()))
        return true;
    return false;
}

bool PuntoOrdenable::operator==(PuntoOrdenable p1) const{
    if(posicion.X()==p1.X() ||
       (posicion.X() == p1.X() && posicion.Y()==p1.Y()) ||
       (posicion.X() == p1.X() && posicion.Y() == p1.Y() && posicion.Z()==p1.Z()))
        return true;
    return false;
}

