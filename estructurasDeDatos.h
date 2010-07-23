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
#include "lib/wrap/gui/trackball.h"
#include "lib/vcg/complex/edgemesh/update/bounding.h"


#include "voxel.h"

/// declaring edge and face type
class CEdge;
class CFace;

/// compositing wanted proprieties
class CVertex : public vcg::VertexSimp2< CVertex, CEdge, CFace, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::Color4b, vcg::vertex::BitFlags>{};
class CEdge   : public vcg::EdgeSimp1 < CVertex, CEdge, vcg::edge::VertexRef> {};
class CFace   : public vcg::FaceSimp2<   CVertex, CEdge, CFace, vcg::face::VertexRef, vcg::face::Normal3f, vcg::face::Color4b, vcg::face::BitFlags > {};
class CMesh   : public vcg::tri::TriMesh< std::vector<CVertex>, std::vector<CFace> > {};
class CEMesh  : public vcg::edg::EdgeMesh< std::vector<CVertex>, std::vector<CEdge> > {};

typedef vcg::GridStaticPtr<CMesh::FaceType,CMesh::ScalarType> TriMeshGrid;

struct PuntoLista{
    vcg::Point3f punto;
    int posicionSiguiente;
};

struct PuntoContornoLight{
    float distancia;
    float altura;
    float angulo;
    float dist2;
    bool operator< (PuntoContornoLight p1) const{return (angulo < p1.angulo || (angulo == p1.angulo && dist2 < p1.dist2));}
    bool operator<=(PuntoContornoLight p1) const{return ((angulo < p1.angulo || (angulo == p1.angulo && dist2 < p1.dist2)) || (distancia == p1.distancia && altura == p1.altura && angulo == p1.angulo && dist2 == p1.dist2));}
    bool operator> (PuntoContornoLight p1) const{return (angulo > p1.angulo || (angulo == p1.angulo && dist2 > p1.dist2));}
    bool operator>=(PuntoContornoLight p1) const{return ((angulo > p1.angulo || (angulo == p1.angulo && dist2 > p1.dist2)) || (distancia == p1.distancia && altura == p1.altura && angulo == p1.angulo && dist2 == p1.dist2));}
    bool operator!=(PuntoContornoLight p1) const{return (distancia == p1.distancia && altura == p1.altura && angulo == p1.angulo && dist2 == p1.dist2);}
    bool operator==(PuntoContornoLight p1) const{return (distancia == p1.distancia && altura == p1.altura && angulo == p1.angulo && dist2 == p1.dist2);}
};


#endif // ESTRUCTURASDEDATOS_H
