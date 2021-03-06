/**
  * Autor: Juan Miguel Huertas Delgado
  * estructurasDeDatos.h
  *
  * En este archivo se encuentran la mayoria de los "includes" relacionados con la vcgLib para
  * tenerlos todos localizados en el mismo archivo y evitar duplicados.
  * Tambien estan aqui almacenadas
  * las estructuras de datos creadas con la vcgLib de vertices, aristas, caras y mallas, tanto para la
  * visualizacion entera de una malla de caras, como de una malla de aristas unicamente.
  * Adem�s tambien estan en este
  * archivo hay dos estructuras de datos, una de ellas de una lista de puntos (minivectores de 3 parametros)
  * y un punto para el contorno, con varios parametros y con las funciones necesarias para poder usado en un "Map" o "MultiMap"
  */

#ifndef ESTRUCTURASDEDATOS_H
#define ESTRUCTURASDEDATOS_H


/// Opengl related imports
#include <GL/glew.h>
#include <QtOpenGL/QGLWidget>
#include <QTime>

/// vcg imports
#include "lib/vcg/simplex/vertex/base.h"
#include "lib/vcg/simplex/face/base.h"
#include "lib/vcg/complex/trimesh/base.h"
#include "lib/vcg/complex/trimesh/update/bounding.h"
#include "lib/vcg/complex/trimesh/update/normal.h"
#include "lib/vcg/complex/trimesh/create/platonic.h"
#include "lib/vcg/complex/trimesh/allocate.h"
#include "lib/vcg/complex/edgemesh/base.h"
#include "lib/vcg/complex/intersection.h"
#include "lib/vcg/space/line3.h"
#include "lib/vcg/space/color4.h"
#include "lib/vcg/space/triangle3.h"
#include "lib/vcg/space/plane3.h"
#include "lib/vcg/math/base.h"
#include "lib/vcg/space/segment3.h"
#include "lib/vcg/complex/trimesh/clean.h"

/// wrapper imports
#include "lib/wrap/io_trimesh/import.h"
#include "lib/wrap/io_trimesh/export.h"
#include "lib/wrap/gl/trimesh.h"
#include "lib/wrap/gl/pick.h"
#include "lib/wrap/gl/picking.h"
#include "lib/wrap/gl/space.h"
#include "lib/wrap/gui/trackball.h"
#include "lib/wrap/gui/trackmode.h"
#include "lib/vcg/complex/edgemesh/update/bounding.h"

#include "voxel.h"

/**
  * Pre-declaracion de las clases "CEdge" arista y "CFace" cara.
  * Se utilizar�n para la declaraci�n del resto de elementos
  *
  */
class CEdge;
class CFace;

/// @brief Declaracion de la clase vertice, se hace uso de vertice, arista, cara, coordenadas, normales, color, y algunos bit extra
class CVertex : public vcg::VertexSimp2< CVertex, CEdge, CFace, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::Color4b, vcg::vertex::BitFlags>{};
/// @brief Declaracion de la clase arista, usando verice y arista simplemente
class CEdge   : public vcg::EdgeSimp1 < CVertex, CEdge, vcg::edge::VertexRef> {};
/// @brief Declaracion de la clase Cara, usando vertice, arista ,cara, referencia del vertice, normal, color, y algunos bit extra
class CFace   : public vcg::FaceSimp2<   CVertex, CEdge, CFace, vcg::face::VertexRef, vcg::face::Normal3f, vcg::face::Color4b, vcg::face::BitFlags > {};
/// @brief Declaracion de la clase malla, usando un vector de vertices y otro de caras
class CMesh   : public vcg::tri::TriMesh< std::vector<CVertex>, std::vector<CFace> > {};
/// @brief Declaracion de la clase malla de aristas, usando un vector de vertices y otro de aristas
class CEMesh  : public vcg::edg::EdgeMesh< std::vector<CVertex>, std::vector<CEdge> > {};

typedef vcg::GridStaticPtr<CMesh::FaceType,CMesh::ScalarType> TriMeshGrid;


/**
    @brief Estructura auxiliar que permite una forma r�pida de acceder a un punto de contorno, almacenando distancia, altura, angulo y otra distancia auxiliar.
    Tiene implementados los m�todos para poder ser ordenado en funci�n del �ngulo y la distancia auxiliar (para contornos)
*/
struct PuntoContornoLight{
    /** La distancia al eje de rotaci�n*/
    float distancia;
    /** La altura respecto al punto m�s bajo*/
    float altura;
    /** El �ngulo que forma el contorno internamente (NO SE USA)*/
    float angulo;
    /** Una distancia auxiliar para calculo de envolvente (NO SE USA)*/
    float dist2;
    /**
    @brief Operator< que permite comparar dos puntos, sera menor segun el �ngulo, y si es igual, segun la distancia
    @param p1 : PuntoContornoLight con el que se compara
    @return El valor del booleano seg�n lo especificado por esta funci�n
    */
    bool operator< (PuntoContornoLight p1) const{return (angulo < p1.angulo || (angulo == p1.angulo && dist2 < p1.dist2));}

    /**
    @brief Operator<= que permite comparar dos puntos, sera menor o igual segun el �ngulo, y si es igual, segun la distancia
    @param p1 : PuntoContornoLight con el que se compara
    @return El valor del booleano seg�n lo especificado por esta funci�n
    */
    bool operator<=(PuntoContornoLight p1) const{return ((angulo < p1.angulo || (angulo == p1.angulo && dist2 < p1.dist2)) || (distancia == p1.distancia && altura == p1.altura && angulo == p1.angulo && dist2 == p1.dist2));}

    /**
    @brief Operator> que permite comparar dos puntos, sera mayor segun el �ngulo, y si es igual, segun la distancia
    @param p1 : PuntoContornoLight con el que se compara
    @return El valor del booleano seg�n lo especificado por esta funci�n
    */
    bool operator> (PuntoContornoLight p1) const{return (angulo > p1.angulo || (angulo == p1.angulo && dist2 > p1.dist2));}

    /**
    @brief Operator>= que permite comparar dos puntos, sera mayor o igual segun el �ngulo, y si es igual, segun la distancia
    @param p1 : PuntoContornoLight con el que se compara
    @return El valor del booleano seg�n lo especificado por esta funci�n
    */
    bool operator>=(PuntoContornoLight p1) const{return ((angulo > p1.angulo || (angulo == p1.angulo && dist2 > p1.dist2)) || (distancia == p1.distancia && altura == p1.altura && angulo == p1.angulo && dist2 == p1.dist2));}

    /**
    @brief Operator!= que permite comparar dos puntos, ser�n distintos si todos su componentes son iguales
    @param p1 : PuntoContornoLight con el que se compara
    @return El valor del booleano seg�n lo especificado por esta funci�n
    */
    bool operator!=(PuntoContornoLight p1) const{return (distancia == p1.distancia && altura == p1.altura && angulo == p1.angulo && dist2 == p1.dist2);}

    /**
    @brief Operator!= que permite comparar dos puntos, ser�n distintos si todos su componentes son iguales
    @param p1 : PuntoContornoLight con el que se compara
    @return El valor del booleano seg�n lo especificado por esta funci�n
    */
    bool operator==(PuntoContornoLight p1) const{return (distancia == p1.distancia && altura == p1.altura && angulo == p1.angulo && dist2 == p1.dist2);}
};


#endif // ESTRUCTURASDEDATOS_H
