/****************************************************************************
 * VCGLib                                                            o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2007                                                \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.                                                      *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
 * for more details.                                                         *
 *                                                                           *
 ****************************************************************************/
/****************************************************************************
  History

$Log: not supported by cvs2svn $
Revision 1.1  2007/10/18 08:52:06  benedetti
Initial release.


****************************************************************************/

#include <QtGui>
#include "glarea.h"
#include <wrap/qt/trackball.h>
#include <QDebug>

using namespace std;

GLArea::GLArea (QWidget * parent)
          :QGLWidget (parent)
{

    hebra = new hebraCalculoEje(this);
    connect(hebra, SIGNAL(finalizaHebra(QMultiMap<int,Voxel>,
                                        float,
                                        vcg::Point3f,
                                        int,
                                        vcg::Line3f)),
            this, SLOT(calculoAcabado(QMultiMap<int,Voxel>,
                                      float,
                                      vcg::Point3f,
                                      int,
                                      vcg::Line3f)), Qt::DirectConnection);

    connect(hebra, SIGNAL(IniciarBarra()),
            this, SLOT(ReIniciarBarra()), Qt::DirectConnection);
    connect(hebra, SIGNAL(AvanzarBarra(int)),
            this, SLOT(ReAvanzarBarra(int)), Qt::DirectConnection);
    connect(hebra, SIGNAL(AcabarBarra()),
            this, SLOT(ReAcabarBarra()), Qt::DirectConnection);
    connect(hebra, SIGNAL(setStatusBar(QString)),
            this, SLOT(ResetStatusBar(QString)), Qt::DirectConnection);
    connect(hebra, SIGNAL(Imprimir(QString)),
            this, SLOT(ReImprimir(QString)), Qt::DirectConnection);

    numPlanos = 0;
    numContornos = 0;
    numContornosInt = 0;
    ResetearPieza();

    updateGL();

}

void GLArea::ResetearPieza(){
    delete hebra;
    hebra = new hebraCalculoEje(this);
    connect(hebra, SIGNAL(finalizaHebra(QMultiMap<int,Voxel>,
                                        float,
                                        vcg::Point3f,
                                        int,
                                        vcg::Line3f)),
            this, SLOT(calculoAcabado(QMultiMap<int,Voxel>,
                                      float,
                                      vcg::Point3f,
                                      int,
                                      vcg::Line3f)), Qt::DirectConnection);
    normales.clear();
    puntosObtenidos.clear();
    puntosExtras.clear();
    contorno.clear();
    carasSeleccionadas.clear();
    contornoOrdenado.clear();
    contornoAlturas.clear();
    voxels.clear();
    mesh.Clear();
    mesh2.Clear();
    meshExt.Clear();
    /*
    for(int i=0; i<numPlanos; ++i)
        puntosDibujar->clear();
    delete [] puntosDibujar;*/
    for(int i=0; i<numContornos; ++i)
        contornos[i].clear();
    for(int i=0; i<numContornosInt; ++i){
        contornosInt[i].clear();
    }
    if(numContornos > 0){
        delete [] contornos;
        delete [] contornosInt;
    }
    MAXCONTORNOS = 100;
    MAXCONTORNOSINT = 100;
    contornos = new QList<puntoContorno> [100];
    contornosInt = new QList<puntoContorno> [100];
    numContornos = 0;
    numContornosInt = 0;
    alreves = 1;
    zoom = 1;
    updown = leftright = alturaExtra = alturaExtraOld = anguloOld = anguloPieza = 0;
    botonIzquierdo = false;
    puntoAlto.setAltura(-100000000);
    estadoDelCalculo = -1;
    antiguoTamanioVoxel = -1;
    drawmode= SMOOTH;
    cantidadDeIntersecciones = 1;
    filtroNormales = 1;
    nivelVoxels = 9;
    numPlanos = 2;
    aristacas = new CEMesh [numPlanos];
    planosIntersecta = new bool[numPlanos];
    for(int i=0; i<numPlanos; ++i)
        planosIntersecta[i] = false;

    vcg::Point3f ejeOrigen (0,-0.8,20);
    vcg::Point3f ejeDireccion (0,0,-40);
    ejeFinal.Set(ejeOrigen,ejeDireccion);
    //todos estos en falso
    dibujarEje = dibujarEjeDePrueba = dibujarAristas = dibujarVoxels =
        contornoExterior = contornoInterior = invertido =
        piezaCargada = ransacCalculado = cambiaDibujo =
        pintarContorno = pintar = planoInterseccion =
        ejeDibujando = normalesObtenidas = seleccionarActivado = moverPieza = false;
    modoDibujado = true;
    mallaGirada = false;
}

void GLArea::CerrarProyecto(){
    ResetearPieza();
    emit setTextoInformacion("");
    emit setStatusBar(tr("Proyecto cerrado"));
}

void GLArea::cambioModo(int mode){
        drawmode=DrawMode(mode);
        updateGL();
}

void GLArea::loadMesh(QString fileName, int tipoArchivo)
{
   emit hebra->inicializarSemilla();
   if(piezaCargada) ResetearPieza();
   piezaCargada = true;
   nombreArchivo.append(fileName);
   if(tipoArchivo==0){
       int err=vcg::tri::io::ImporterPLY<CMesh>::Open(mesh,(fileName.toStdString()).c_str());
            if(err!=0){
              const char* errmsg=vcg::tri::io::ImporterPLY<CMesh>::ErrorMsg(err);
              QMessageBox::warning(this,tr("Error importando el modelo PLY"),QString(errmsg));
            }
            initMesh("Importado \""+fileName+"\".");
            vcg::tri::io::ImporterPLY<CMesh>::Open(meshExt,(fileName.toStdString()).c_str());
            initMeshSecundarias();
    }else{
        int err=vcg::tri::io::Importer<CMesh>::Open(mesh,(fileName.toStdString()).c_str());
        if(err!=0){
        }
            initMesh("Importado \""+fileName+"\".");
    }
}

void GLArea::saveMesh(QString fileName, int tipoArchivo)
{
   if(tipoArchivo==0){
       int err=vcg::tri::io::ExporterPLY<CMesh>::Save(figuraFinal,(fileName.toStdString()).c_str());
            if(err!=0){
              const char* errmsg=vcg::tri::io::ExporterPLY<CMesh>::ErrorMsg(err);
              QMessageBox::warning(this,tr("Error exportando el modelo PLY"),QString(errmsg));
            }
            initMesh("Exportado \""+fileName+"\".");
    }else{
        int err=vcg::tri::io::Exporter<CMesh>::Save(figuraFinal,(fileName.toStdString()).c_str());
        if(err!=0){
        }
            initMesh("Exportado \""+fileName+"\".");
    }
}

void GLArea::cancelarSalir()
{
}

void GLArea::initMeshSecundarias(){
    int dup = vcg::tri::Clean<CMesh>::RemoveDuplicateVertex(meshExt);
    int unref =  vcg::tri::Clean<CMesh>::RemoveUnreferencedVertex(meshExt);
    if (dup > 0 || unref > 0){
           emit Imprimir(tr("Removed "));
           emit Imprimir(QString::number(dup));
           emit Imprimir(tr(" duplicate and "));
           emit Imprimir(QString::number(unref));
           emit Imprimir(tr(" unreferenced vertices from meshExt"));
    }
    // update bounding box
    vcg::tri::UpdateBounding<CMesh>::Box(meshExt);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(meshExt);
    CMesh::VertexIterator vi=meshExt.vert.begin();
    do{
    (*vi).P()=CMesh::CoordType ((*vi).P().X()-meshExt.bbox.Center().X(),
                                (*vi).P().Y()-meshExt.bbox.Center().Y(),
                                (*vi).P().Z()-meshExt.bbox.Center().Z()); ++vi;
    }while(vi!=meshExt.vert.end());
    vcg::tri::UpdateBounding<CMesh>::Box(meshExt);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(meshExt);
    float d=1.0f/meshExt.bbox.Diag();

    vi=meshExt.vert.begin();
    do{
    (*vi).P()=CMesh::CoordType ((*vi).P().X()*d,
                                (*vi).P().Y()*d,
                                (*vi).P().Z()*d);++vi;
    }while(vi!=meshExt.vert.end());
    vcg::tri::UpdateBounding<CMesh>::Box(meshExt);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(meshExt);
}

void GLArea::initMesh(QString message)
{
    int dup = vcg::tri::Clean<CMesh>::RemoveDuplicateVertex(mesh);
    int unref =  vcg::tri::Clean<CMesh>::RemoveUnreferencedVertex(mesh);
    if (dup > 0 || unref > 0){
           emit Imprimir(tr("Removed "));
           emit Imprimir(QString::number(dup));
           emit Imprimir(tr(" duplicate and "));
           emit Imprimir(QString::number(unref));
           emit Imprimir(tr(" unreferenced vertices from mesh"));
    }
    pintar = true;
    dibujarNormales = normalesObtenidas = ejeDibujando = planoInterseccion = false;
    // update bounding box
    vcg::tri::UpdateBounding<CMesh>::Box(mesh);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(mesh);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(mesh);
    CMesh::VertexIterator vi=mesh.vert.begin();
    do{
    (*vi).P()=CMesh::CoordType ((*vi).P().X()-mesh.bbox.Center().X(),
                                (*vi).P().Y()-mesh.bbox.Center().Y(),
                                (*vi).P().Z()-mesh.bbox.Center().Z()); ++vi;
    }while(vi!=mesh.vert.end());
    vcg::tri::UpdateBounding<CMesh>::Box(mesh);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(mesh);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(mesh);
    float d=1.0f/mesh.bbox.Diag();
    escala = d;

    vi=mesh.vert.begin();
    do{
    (*vi).P()=CMesh::CoordType ((*vi).P().X()*d,
                                (*vi).P().Y()*d,
                                (*vi).P().Z()*d); ++vi;
    }while(vi!=mesh.vert.end());
    vcg::tri::UpdateBounding<CMesh>::Box(mesh);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(mesh);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(mesh);
    // Initialize the opengl wrapper
    glWrap.m = &mesh;
    numCaras = mesh.face.size();
    //distanciaMinima = diagonal;
    glWrap.Update();
    diagonal = glWrap.m->bbox.Diag();
    cout << "\nDimensiones" << glWrap.m->bbox.DimX() << "\n"<< glWrap.m->bbox.DimY() << "\n"<< glWrap.m->bbox.DimZ() << "\n";
    updateGL();
    emit setTextoInformacion(QString::number(glWrap.m->face.size())+" caras. "+
                             QString::number(glWrap.m->vert.size())+" vertices.");
    emit setStatusBar(message);
}

void GLArea::cambioExtInt(){
    CMesh::VertexIterator vi, vi2;
    CMesh::CoordType punto;
    vcg::Quaternionf q1;

    q1.FromAxis(PI,vcg::Point3f(0,1,0));
    vi = meshExt.vert.begin();
    do{
    punto=CMesh::CoordType ((*vi).P().X(),
                            (*vi).P().Y(),
                            (*vi).P().Z());
    punto=q1.Rotate(punto);
    (*vi).P()=CMesh::CoordType (punto.X(),
                                punto.Y(),
                                punto.Z());
    ++vi;
    }while(vi!=meshExt.vert.end());
    vcg::tri::UpdateBounding<CMesh>::Box(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(meshExt);
    glWrapRaro.m = &meshExt;
    glWrapRaro.Update();
}

void GLArea::invertirMalla(){
    CMesh::VertexIterator vi, vi2;
    CMesh::CoordType punto;
    vcg::Quaternionf q1;

    q1.FromAxis(PI,vcg::Point3f(0,0,1));
    vi = meshExt.vert.begin();
    do{
    punto=CMesh::CoordType ((*vi).P().X(),
                            (*vi).P().Y(),
                            (*vi).P().Z());
    punto=q1.Rotate(punto);
    (*vi).P()=CMesh::CoordType (punto.X(),
                                punto.Y(),
                                punto.Z());
    ++vi;
    }while(vi!=meshExt.vert.end());
    vcg::tri::UpdateBounding<CMesh>::Box(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(meshExt);
    glWrapRaro.m = &meshExt;
    glWrapRaro.Update();
}

void GLArea::girarMalla(){
    float fi, theta, aux;
    ejeFinal.Direction().ToPolar(aux,theta,fi);
    CMesh::VertexIterator vi, vi2;
    CMesh::CoordType punto;
    vcg::Quaternionf q1,q2,q3,q5,q6;
    float max = -10000, min = 10000;

    q1.FromAxis(theta,vcg::Point3f(0,1,0));
    q2.FromAxis(  -fi,vcg::Point3f(0,0,1));
    q3.FromAxis(PI/2.,vcg::Point3f(0,0,1));
    q5.FromAxis(anguloRotar*PI/180.,vcg::Point3f(0,1,0));
    vi = meshExt.vert.begin();
    do{
    punto=CMesh::CoordType ((*vi).P().X()-ejeFinal.Origin().X(),
                            (*vi).P().Y()-ejeFinal.Origin().Y(),
                            (*vi).P().Z()-ejeFinal.Origin().Z());

    punto=q1.Rotate(punto);
    punto=q2.Rotate(punto);
    punto=q3.Rotate(punto);
    punto=CMesh::CoordType (punto.X(),
                            punto.Y(),
                            punto.Z());
    punto=q5.Rotate(punto);

    (*vi).P()=CMesh::CoordType (punto.X(),
                                punto.Y(),
                                punto.Z());
    if(max < punto.Y())
        max = punto.Y();
    if(min > punto.Y())
        min = punto.Y();
    ++vi;
    }while(vi!=meshExt.vert.end());

    alturaCentrado = -(max-min)/2;
     vi = meshExt.vert.begin();
    do{
    (*vi).P()=CMesh::CoordType ((*vi).P().X(),
                                (*vi).P().Y()+alturaCentrado,
                                (*vi).P().Z());
    ++vi;
    }while(vi!=meshExt.vert.end());

    vcg::tri::UpdateBounding<CMesh>::Box(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(meshExt);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(meshExt);
    glWrapRaro.m = &meshExt;
    glWrapRaro.Update();
    mallaGirada=true;
}

void GLArea::initializeGL ()
{
  glClearColor(0, 0, 0, 0);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void GLArea::resizeGL (int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  initializeGL();
 }

void GLArea::verDibujoArqueologico(){
    pintarContorno = true;
    pintar = false;
    dibujarEje = false;
    updateGL();
}

void GLArea::verModeloReconstruido(){
    pintarContorno = false;
    pintar = true;
    cambiaDibujo = true;
    dibujarEje = false;
    updateGL();
}

void GLArea::paintGL ()
{
    GLfloat posicionLuz[3];
    posicionLuz[0] = 0;
    posicionLuz[2] = 0;
    posicionLuz[1] = 100000000;// Create light components
    //glLightfv(GL_LIGHT0, GL_POSITION,posicionLuz);
    aspectRatio = 1.0*GLArea::width()/ GLArea::height()*1.0;
    if(pintarContorno){
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if(GLArea::width() > GLArea::height())
            glOrtho(-(1/zoom)*aspectRatio, (1/zoom)*aspectRatio, -(1/zoom), (1/zoom), 0.1, 10);
        else
            glOrtho(-(1/zoom), (1/zoom), -(1/zoom)/aspectRatio, (1/zoom)/aspectRatio, 0.1, 10);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();//Dibujar el contorno cutre
        gluLookAt(0,0,5,   0,0,0,   0,1,0);
        glColor3f(0.2,0.2,0.2);/*
        if(GLArea::width() > GLArea::height()){
            renderText(-(1/zoom)*aspectRatio+0.1/zoom,(1/zoom)-0.1/zoom,0,"ESCALA DE LA PIEZA",this->font());
        }else{
            renderText(-(1/zoom)+0.1/zoom,(1/zoom)/aspectRatio-0.1/zoom,0,"ESCALA DE LA PIEZA",this->font());
        }*/

        track.Reset();
        track.center=vcg::Point3f(0, 0, 0);
        track.radius= 10000;
        track.GetView();
        track.Apply(false);
        glPushMatrix();
            //PANCHITO 1
            glTranslatef(leftright,updown,0);

            glDisable(GL_LIGHTING);
            glColor3f(0,0,0);
            glEnable(GL_LINE_SMOOTH);
            glLineWidth(0.2);
            glBegin(GL_LINES);
                glVertex3f(0,maxGlobal,0);
                glVertex3f(0,minGlobal,0);

                glVertex3f(0,maxGlobal,0);
                glVertex3f(-distanciaMaxGlobal,maxGlobal,0);
            glEnd();
            renderText(-distanciaMaxGlobal,maxGlobal+0.01,0,QString::number(distanciaMaxGlobal/escala),this->font());//QFont("helvetica", 12, QFont::Bold, TRUE));
            glPushMatrix();
                glRotatef(90,0,0,1);
                renderText(0,0,0,QString::number((maxGlobal - minGlobal)/escala),this->font());
            glPopMatrix();
            renderText(0,minGlobal*1.1,0,"ESCALA",this->font());
            glDisable(GL_LINE_SMOOTH);
            glColor3f(0,0,0);
            glEnd();
            glLineWidth(1);
            glBegin(GL_LINE_STRIP);
                foreach(PuntoContornoLight puntoActual, puntosObtenidos){
                    glVertex3f(-puntoActual.distancia,
                               puntoActual.altura,
                               0);
                }
            glEnd();
            glEnable(GL_LIGHTING);
            glRotatef(-90+anguloPieza,0,1,0);
            glWrapRaro.Draw<vcg::GLW::DMSmooth,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
            glPopMatrix();
        track.DrawPostApply();
    }
    if(pintar){
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(40, GLArea::width()/(float)GLArea::height(), 0.1, 100);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0,0,5,   0,0,0,   0,1,0);
        track.center=vcg::Point3f(0, 0, 0);
        track.radius= 1;
        track.GetView();
        track.Apply(false);
        glPushMatrix();
        //float d=1.0f/mesh.bbox.Diag();
       // vcg::glScale(d);
        if(cambiaDibujo){ //Dibujar con la pieza reconstruida
                switch(drawmode)
                {
                  case SMOOTH:
                        glFigura.Draw<vcg::GLW::DMSmooth,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();//DMSmooth,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case POINTS:
                        glFigura.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();
                        break;
                  case WIRE:
                        glFigura.Draw<vcg::GLW::DMWire,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();//DMWire,     vcg::GLW::CMNone,vcg::GLW::TMNone> ();
                        break;
                  case FLATWIRE:
                        glFigura.Draw<vcg::GLW::DMFlatWire,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();//DMFlatWire, vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case HIDDEN:
                        glFigura.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();//DMHidden,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case FLAT:
                        glFigura.Draw<vcg::GLW::DMFlat,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();//DMFlat,     vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  default:
                        break;
                 }
        }else{ //Dibujar ANTES de reconstruir
            if(planoInterseccion) //Dibujar plano de prueba
                glWrapPlano.Draw<vcg::GLW::DMFlat, vcg::GLW::CMPerFace, vcg::GLW::TMNone> ();
            if(dibujarEje){
                glPushMatrix();
                glColor3f(1,0,0);
                glDisable(GL_LIGHTING);
                glEnable(GL_LINE_SMOOTH);
                glLineWidth(2);
                if(contornoExterior)
                for(int i=0; i<numContornos; ++i){
                    glBegin(GL_LINE_STRIP);
                    foreach(puntoContorno puntoActual, contornos[i])
                        glVertex3f(puntoActual.getPosicion().X(),puntoActual.getPosicion().Y(),puntoActual.getPosicion().Z());
                    glEnd();
                }
                if(contornoInterior)
                    for(int i=0; i<numContornosInt; ++i){
                        glBegin(GL_LINE_STRIP);
                        foreach(puntoContorno puntoActual, contornosInt[i])
                            glVertex3f(puntoActual.getPosicion().X(),puntoActual.getPosicion().Y(),puntoActual.getPosicion().Z());
                        glEnd();
                    }
                glEnable(GL_LIGHTING);
                glDisable(GL_LINE_SMOOTH);

                float fi, theta, aux;
                ejeFinal.Direction().ToPolar(aux,theta,fi);
                fi = fi*180./PI;
                theta = theta*180./PI;
                glColor3f(0,0,1);
                glColor3f(1,0,0);
                if(invertido)
                    glRotatef(180,0,0,1);
                if(contornoExterior)
                    glRotatef(anguloRotar,0,1,0);
                if(contornoInterior)
                    glRotatef(anguloRotar+180,0,1,0);

                glTranslatef(0,alturaCentrado+alturaExtra,0);
                glRotatef(0,1,0,0);
                glRotatef(90,0,0,1);
                glRotatef(-fi,0,0,1);
                glRotatef(theta,0,1,0);
                glTranslatef(-ejeFinal.Origin().X(),-ejeFinal.Origin().Y(),-ejeFinal.Origin().Z());
                glPushMatrix();
                glDisable(GL_LIGHTING);
                glColor3f(0,1,0);
                glBegin(GL_LINES);
                glVertex3f(ejeFinal.Origin().X() - 100.*ejeFinal.Direction().X(),
                           ejeFinal.Origin().Y() - 100.*ejeFinal.Direction().Y(),
                           ejeFinal.Origin().Z() - 100.*ejeFinal.Direction().Z());
                glVertex3f(ejeFinal.Origin().X() + 100.*ejeFinal.Direction().X(),
                           ejeFinal.Origin().Y() + 100.*ejeFinal.Direction().Y(),
                           ejeFinal.Origin().Z() + 100.*ejeFinal.Direction().Z());
                glEnd();
                glColor3f(0,1,0);
                glEnable(GL_POINT_SMOOTH);
                glPointSize(5);
                glBegin(GL_POINTS);
                glVertex3f(ejeFinal.Origin().X(),
                           ejeFinal.Origin().Y(),
                           ejeFinal.Origin().Z());
                glEnd();
                glColor3f(1,0,0);
                glBegin(GL_POINTS);
                glVertex3f(ejeFinal.Origin().X() + ejeFinal.Direction().X(),
                           ejeFinal.Origin().Y() + ejeFinal.Direction().Y(),
                           ejeFinal.Origin().Z() + ejeFinal.Direction().Z());
                glEnd();
                glDisable(GL_POINT_SMOOTH);
                glPointSize(1);

                if(dibujarEjeDePrueba){
                    glColor3f(0,0,1);
                    glBegin(GL_LINES);
                    glVertex3f(ejeDePrueba.Origin().X() - 100.*ejeDePrueba.Direction().X(),
                               ejeDePrueba.Origin().Y() - 100.*ejeDePrueba.Direction().Y(),
                               ejeDePrueba.Origin().Z() - 100.*ejeDePrueba.Direction().Z());
                    glVertex3f(ejeDePrueba.Origin().X() + 100.*ejeDePrueba.Direction().X(),
                               ejeDePrueba.Origin().Y() + 100.*ejeDePrueba.Direction().Y(),
                               ejeDePrueba.Origin().Z() + 100.*ejeDePrueba.Direction().Z());
                    glEnd();
                glColor3f(0,0,1);
                glEnable(GL_POINT_SMOOTH);
                glPointSize(5);
                glBegin(GL_POINTS);
                glVertex3f(ejeDePrueba.Origin().X(),
                           ejeDePrueba.Origin().Y(),
                           ejeDePrueba.Origin().Z());
                glEnd();
                glColor3f(1,0,0);
                glBegin(GL_POINTS);
                glVertex3f(ejeDePrueba.Origin().X() + ejeDePrueba.Direction().X(),
                           ejeDePrueba.Origin().Y() + ejeDePrueba.Direction().Y(),
                           ejeDePrueba.Origin().Z() + ejeDePrueba.Direction().Z());
                glEnd();
                glDisable(GL_POINT_SMOOTH);
                glPointSize(1);
                }
                glEnable(GL_LIGHTING);
                glPopMatrix();
            }
            if(dibujarVoxels){ //Dibujar vóxeles
                    glDisable(GL_LIGHTING);
                    glBegin(GL_POINTS);
                    foreach(Voxel voxel, voxels){
                        if(voxel.contador() >= cantidadDeIntersecciones){
                            if(voxel.contador() == cantidadDeIntersecciones)
                                glColor3f(0.93,0.51,0.93);
                            else if(voxel.contador() <= cantidadDeIntersecciones + valorColorVoxel)
                                glColor3f(0.3,0,0.51);
                            else if(voxel.contador() <= cantidadDeIntersecciones + 2*valorColorVoxel)
                                glColor3f(0,0,1);
                            else if(voxel.contador() <= cantidadDeIntersecciones + 3*valorColorVoxel)
                                glColor3f(0,1,0);
                            else if(voxel.contador() <= cantidadDeIntersecciones + 4*valorColorVoxel)
                                glColor3f(1,1,0);
                            else if(voxel.contador() <= cantidadDeIntersecciones + 5*valorColorVoxel)
                                glColor3f(1,0.55,0);
                            else
                                glColor3f(1,0,0);
                            glVertex3f((voxel.posicion.X() - valorMedio)*nodoRaiz.X(),
                                       (voxel.posicion.Y() - valorMedio)*nodoRaiz.Y(),
                                       (voxel.posicion.Z() - valorMedio)*nodoRaiz.Z());
                        }
                    }
                    glEnd();
                    glEnable(GL_LIGHTING);
            }
            // vcg::glTranslate(-glWrap.m->bbox.Center());
            if(dibujarNormales){ //Dibuja las normales
                glPushMatrix();
                glColor3f(1,1,1);
                    glDisable(GL_LIGHTING);
                glBegin(GL_LINES);
                foreach(vcg::Line3f normal, normales){
                    glVertex3f(normal.Origin().X(),normal.Origin().Y(),normal.Origin().Z());
                    glVertex3f(normal.Origin().X() + 100.*normal.Direction().X(),
                               normal.Origin().Y() + 100.*normal.Direction().Y(),
                               normal.Origin().Z() + 100.*normal.Direction().Z());
                   // emit setStatusBar(QString::number(voxel.pos().X()));
                }
                glEnd();
                    glEnable(GL_LIGHTING);
                glPopMatrix();
            }
            glPushMatrix();
            glPopMatrix();
            if(false){//dibujarAristas){
                glColor3f(1,0,0);
                glEnable(GL_POINT_SMOOTH);
                glDisable(GL_LIGHTING);
                glPointSize(2);
                for(int j=0; j<numPlanos; ++j){
                    glBegin(GL_POINTS);
                    for(int i=0; i<puntosDibujar[j].size(); ++i){
                            glVertex3f(puntosDibujar[j].value(i).X(),puntosDibujar[j].value(i).Y(),puntosDibujar[j].value(i).Z());
                    }
                    glEnd();
                }
                glColor3f(0,1,0);
                glPointSize(10);
                glBegin(GL_POINTS);
                for(int i=0; i< numPlanos; ++i)
                    glVertex3f(centro[i].X(),centro[i].Y(),centro[i].Z());
                glEnd();
                glPointSize(1);
                glDisable(GL_POINT_SMOOTH);
                glBegin(GL_LINES);
                for(int i=0; i< numPlanos; ++i)
                    glVertex3f(centro[i].X(),centro[i].Y(),centro[i].Z());
                glEnd();
                glColor3f(0,0,1);
                glPointSize(10);
                glBegin(GL_POINTS);
                for(int i=0; i< numPlanos; ++i)
                    glVertex3f(nuevoCentro[i].X(),nuevoCentro[i].Y(),nuevoCentro[i].Z());
                glEnd();
                glPointSize(1);
                glDisable(GL_POINT_SMOOTH);
                glBegin(GL_LINES);
                for(int i=0; i< numPlanos; ++i)
                    glVertex3f(nuevoCentro[i].X(),nuevoCentro[i].Y(),nuevoCentro[i].Z());
                glEnd();
                glEnable(GL_LIGHTING);


            }
            if(modoDibujado){
                if(false){//Dibujar bounding box
                    glEnable(GL_POINT_SMOOTH);
                    glDisable(GL_LIGHTING);
                    glPointSize(3);
                    glColor3f(1,0,0);
                    glBegin(GL_POINTS);
                            glVertex3f(-glWrap.m->bbox.DimX()/2.,-glWrap.m->bbox.DimY()/2.,-glWrap.m->bbox.DimZ()/2.);
                            glVertex3f(-glWrap.m->bbox.DimX()/2.,-glWrap.m->bbox.DimY()/2., glWrap.m->bbox.DimZ()/2.);
                            glVertex3f(-glWrap.m->bbox.DimX()/2., glWrap.m->bbox.DimY()/2.,-glWrap.m->bbox.DimZ()/2.);
                            glVertex3f(-glWrap.m->bbox.DimX()/2., glWrap.m->bbox.DimY()/2., glWrap.m->bbox.DimZ()/2.);
                            glVertex3f( glWrap.m->bbox.DimX()/2.,-glWrap.m->bbox.DimY()/2.,-glWrap.m->bbox.DimZ()/2.);
                            glVertex3f( glWrap.m->bbox.DimX()/2.,-glWrap.m->bbox.DimY()/2., glWrap.m->bbox.DimZ()/2.);
                            glVertex3f( glWrap.m->bbox.DimX()/2., glWrap.m->bbox.DimY()/2.,-glWrap.m->bbox.DimZ()/2.);
                            glVertex3f( glWrap.m->bbox.DimX()/2., glWrap.m->bbox.DimY()/2., glWrap.m->bbox.DimZ()/2.);
                    glEnd();
                    glDisable(GL_POINT_SMOOTH);
                    glEnable(GL_LIGHTING);
                }
                switch(drawmode){ //Dibuja pieza sin reconstruir
                  case SMOOTH:
                        glWrap.Draw<vcg::GLW::DMSmooth,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case POINTS:
                        glWrap.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();
                        break;
                  case WIRE:
                        glWrap.Draw<vcg::GLW::DMWire,     vcg::GLW::CMNone,vcg::GLW::TMNone> ();
                        break;
                  case FLATWIRE:
                        glWrap.Draw<vcg::GLW::DMFlatWire, vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case HIDDEN:
                        glWrap.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case FLAT:
                        glWrap.Draw<vcg::GLW::DMFlat,     vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  default:
                        break;
                }
            }
            if(dibujarEje)
                glPopMatrix();
        }
        glPopMatrix();
        track.DrawPostApply();
    }
}

void GLArea::keyReleaseEvent (QKeyEvent * e)
{
  e->ignore ();
  if (e->key () == Qt::Key_Control)
    track.ButtonUp (QT2VCG (Qt::NoButton, Qt::ControlModifier));
  if (e->key () == Qt::Key_Shift)
    track.ButtonUp (QT2VCG (Qt::NoButton, Qt::ShiftModifier));
  if (e->key () == Qt::Key_Alt)
    track.ButtonUp (QT2VCG (Qt::NoButton, Qt::AltModifier));
  updateGL ();
}

void GLArea::keyPressEvent (QKeyEvent * e)
{
  e->ignore ();
  if (e->key () == Qt::Key_Control)
    track.ButtonDown (QT2VCG (Qt::NoButton, Qt::ControlModifier));
  if (e->key () == Qt::Key_Shift)
    track.ButtonDown (QT2VCG (Qt::NoButton, Qt::ShiftModifier));
  if (e->key () == Qt::Key_Alt)
    track.ButtonDown (QT2VCG (Qt::NoButton, Qt::AltModifier));
  if(pintarContorno){
      if (e->key () == Qt::Key_Plus)
          zoom+=0.2;
      if (e->key () == Qt::Key_Minus)
          zoom-=0.2;
      if (e->key () == Qt::Key_Right)
          leftright+=0.1;
      if (e->key () == Qt::Key_Left)
          leftright-=0.1;
      if (e->key () == Qt::Key_Up)
          updown+=0.1;
      if (e->key () == Qt::Key_Down)
          updown-=0.1;
  }
  updateGL ();
}

void GLArea::mousePressEvent (QMouseEvent * e)
{
  e->accept ();
  setFocus ();
  if(moverPieza){
      anguloOld = anguloPieza;
      ratonXOld = e->x();
  }else if(seleccionarActivado){
        if(e->buttons()&Qt::RightButton){
            if(dibujarEje){
                //alturaExtraOld = alturaExtra;
                track.MouseDown (e->x (), height ()- e->y (), QT2VCG (Qt::MidButton, e->modifiers ()));
            }else
                track.MouseDown (e->x (), height () - e->y (), QT2VCG (Qt::MidButton, e->modifiers ()));
            botonIzquierdo = false;
        }else{
        if(contornoExterior)
            numContornos++;
        if(contornoInterior)
            numContornosInt++;
        ComprobarContornos();
            clickado(e->x(), height() - e->y());
            botonIzquierdo = true;
        }
  }else
      if(dibujarEje){
          alturaExtraOld = alturaExtra;
          ratonYOld = e->y();
          track.MouseDown (e->x (), height ()/2, QT2VCG (e->button (), e->modifiers ()));
      }else
          track.MouseDown (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));

  updateGL ();
}

void GLArea::mouseMoveEvent (QMouseEvent * e)
{
  if (e->buttons ()) {
  if(moverPieza){
    anguloPieza = anguloOld + (e->x() - ratonXOld);
    if(anguloPieza < -45) anguloPieza = -45;
    if(anguloPieza > 45) anguloPieza = 45;
  }
      if(seleccionarActivado){
        if(botonIzquierdo)
            clickado(e->x(), height() - e->y());
        else if(dibujarEje){
                track.MouseMove(e->x (), height () - e->y ());
            }else{
                track.MouseMove(e->x (), height () - e->y ());
            }
        }else if(dibujarEje){
            alturaExtra = alturaExtraOld - alreves*(e->y() - ratonYOld)/height();
            track.MouseMove(e->x (), height ()/2);
        }else{
            track.MouseMove(e->x (), height () - e->y ());
        }
    }else{
        emit RatonEnGLArea();
    }

    updateGL ();
}

void GLArea::mouseReleaseEvent (QMouseEvent * e)
{
    if(seleccionarActivado)
        if(botonIzquierdo)
            clickado(e->x(), height() - e->y());
        else if(dibujarEje){
            track.MouseUp (e->x (), height () - e->y (), QT2VCG (Qt::MidButton, e->modifiers ()));
        }else
            track.MouseUp (e->x (), height () - e->y (), QT2VCG (Qt::MidButton, e->modifiers ()));
    else if(dibujarEje){
        track.MouseUp (e->x (), height ()/2, QT2VCG (e->button (), e->modifiers ()));
    }else
        track.MouseUp (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));

  updateGL ();
}

void GLArea::wheelEvent (QWheelEvent * e)
{
  const int WHEEL_STEP = 120;
  track.MouseWheel (e->delta () / float (WHEEL_STEP), QTWheel2VCG (e->modifiers ()));
  updateGL ();
}

void GLArea::calcularEje(){
    int * parametros = new int [7];
    if(numCaras < 10000)
        parametros[0] = 10;
    else if(numCaras < 100000)
        parametros[0] = 50;
    else if(numCaras < 1000000)
        parametros[0] = 1000;
    else
        parametros[0] = 1500;
    parametros[1] = 60;
    parametros[2] = 90;
    parametros[3] = 50;
    parametros[4] = 1000;
    parametros[5] = 10;
    parametros[6] = 0;
    emit CrearConfiguracionEje(parametros);
}

void GLArea::calcularInicializarEje(bool metodo){
    int * parametros = new int [7];
    parametros[1] = 60;
    parametros[2] = 90;
    parametros[3] = 50;
    parametros[4] = 1000;
    parametros[5] = 10;
    parametros[6] = 0;
    if(numCaras < 10000)
        parametros[0] = 10;
    else if(numCaras < 100000)
        parametros[0] = 50;
    else if(numCaras < 1000000){
        parametros[0] = 2000;
        parametros[1] = 70;
        parametros[3] = 40;
    }else
        parametros[0] = 2500;
    inicializarEje(parametros[0],parametros[1],parametros[2],parametros[3],parametros[4],parametros[5],parametros[6],true,true,parametros[9],metodo);
}

void GLArea::inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje, bool refinarEje, bool metodo){
    metodoCluster = metodo;
    porcentajeDeseado = porcentaje/100.;
    limiteVoxelesInferior = limInf;
    limiteVoxelesIntermedio = limIntermedio/100.;
    limiteVoxelesEje = limiteIntersec/100.;
    limiteIteraciones = limiteIteracion;
    amplitudVoxels = amplitud;
    amplitudMinima = amplitudMin;

    parametrosCargados[0] = limInf;
    parametrosCargados[1] = limIntermedio;
    parametrosCargados[2] = porcentaje;
    parametrosCargados[3] = limiteIntersec;
    parametrosCargados[4] = limiteIteracion;
    parametrosCargados[5] = amplitud;
    parametrosCargados[6] = amplitudMin;
    calcularV = parametrosCargados[7] = calcularVoxels;
    parametrosCargados[8] = calcularEje;
    ransacCalculado = calcularEje;
    realizarGenetico = refinarEje;

    if(!calcularVoxels && !calcularEje && !realizarGenetico){
        tamanioOrientacionNormales = 360;
        orientacionNormales = new int * [tamanioOrientacionNormales];
        for(int i = 0; i < tamanioOrientacionNormales; ++i){
             orientacionNormales[i] = new int [tamanioOrientacionNormales];
             for(int j=0; j < tamanioOrientacionNormales; ++j)
                orientacionNormales[i][j]=0;
        }
        //Obtener normales
        ObtenerNormales();
        emit Imprimir("????" + QString::number(numNormales) + "!!!!");
        float theta, fi;
        int maximo = 0, mTheta, mFi, total = 0;
        foreach(vcg::Line3f normal, normales){
        //Para cada normal pasarlo a polares
        total++;
        fi = atan2(normal.Direction().X(),normal.Direction().Y());
        theta = sqrt(normal.Direction().X()*normal.Direction().X()+
                    normal.Direction().Y()*normal.Direction().Y()+
                    normal.Direction().Z()*normal.Direction().Z());
        theta = normal.Direction().Z()/theta;
        theta = theta<0?-acos(-theta):acos(theta);
        fi = fi+PI;
        fi = 180.*fi/PI;
        theta = theta+PI;
        theta = 180.*theta/PI;
        //Incrementar orientacionNormales[alfa][beta]++
        orientacionNormales[(int)theta][(int)fi]++;
        if(orientacionNormales[(int)theta][(int)fi]>maximo)
            maximo = orientacionNormales[(int)theta][(int)fi];
            mTheta = (int) theta;
            mFi = (int) fi;
        }
        //Imprimir resultado final en salida2
        ImprimirHistorigrama(tr(" "));
        for(int j = 0; j < tamanioOrientacionNormales; ++j)
            ImprimirHistorigrama(";"+QString::number(j));
        for(int i = 0; i < tamanioOrientacionNormales; ++i){
            ImprimirHistorigrama("\n"+QString::number(i));
            for(int j=0; j < tamanioOrientacionNormales; ++j)
                ImprimirHistorigrama(";"+QString::number(orientacionNormales[i][j]));
        }
        emit ImprimirHistorigrama("\n\nTotal :" + QString::number(total) + " maximo:" + QString::number(maximo) + "\n\n");
        if(maximo > 50){
            vcg::Point3f direccionFinal;
            direccionFinal.X() = sin(vcg::math::ToRad((float)mFi))*cos(vcg::math::ToRad((float)mTheta));
            direccionFinal.Y() = sin(vcg::math::ToRad((float)mFi))*cos(vcg::math::ToRad((float)mTheta));
            direccionFinal.Z() = cos(vcg::math::ToRad((float)mFi));
            ejeFinal.Set(vcg::Point3f(0,0,0),direccionFinal);
            //dibujarEje = true;
        }
    }else if(!calcularVoxels && !calcularEje){
        calidadDelEje();
    }else{
        if(hebra->isRunning())
            ;//emit AvisoError();
        else{
            hebra->setParametros(parametrosCargados);
            hebra->setOperacion(0,metodoCluster);
            hebra->insertarDatos(glWrap);
            if(!calcularVoxels)
                hebra->insertarVoxels(voxels);
            estadoDelCalculo = 0;
            hebra->start();
        }
    }
}

void GLArea::ObtenerNormales(){
   CMesh::FaceIterator fi=mesh.face.begin();
    vcg::Line3f nueva_normal;
    int contando = 0;
    normalesObtenidas = true;

    nueva_normal.SetOrigin(vcg::Barycenter(*fi));
    nueva_normal.SetDirection(vcg::Normal(*fi));
    normales.append(nueva_normal);
    ++fi;
    while(fi!=mesh.face.end()){
        nueva_normal.SetOrigin(vcg::Barycenter(*fi));
        nueva_normal.SetDirection(vcg::Normal(*fi));
        normales.append(nueva_normal);
        ++fi;
        ++contando;
    }

    numNormales = contando;

    emit setStatusBar(tr("Normales obtenidas"));

}

void GLArea::dibujaNormales(){
    if(!normalesObtenidas){
        ObtenerNormales();
        normalesObtenidas = true;
    }
    dibujarNormales = !dibujarNormales;
    updateGL();
}

void GLArea::seleccionar(){
    seleccionarActivado = !seleccionarActivado;
}

void GLArea::seleccionarExterior(bool valor){
    seleccionarActivado = contornoExterior = valor;
    alturaExtra = 0;
    track.Reset();
    updateGL();
}

void GLArea::seleccionarInterior(bool valor){
    seleccionarActivado = contornoInterior = valor;
    if(mallaGirada) cambioExtInt();
    alturaExtra = 0;
    track.Reset();
    updateGL();
}

void GLArea::Generar(){
    CalcularContorno();
    cambiaDibujo=!cambiaDibujo;
    pintarContorno = !pintarContorno;
    pintar=!pintar;
    updateGL();
}

void GLArea::CalcularContorno(){
    contornoOrdenado.clear();
    PuntoContornoLight puntoInsertando;
    puntoContorno puntoNuevo;
    float alturaMin = 1000000, alturaMax = -1000000;

    for(int i=0; i<numContornos; ++i){
        foreach(puntoContorno p, contornos[i]){
            contornoAlturas.insert(p.altura,p);
            if(p.altura < alturaMin)
                alturaMin = p.altura;
            if(p.altura > alturaMax)
                alturaMax = p.altura;
        }
    }

    for(int i=0; i<numContornosInt; ++i){
        foreach(puntoContorno p, contornosInt[i]){
            contornoAlturasInt.insert(-p.altura,p);
            if(p.altura < alturaMin)
                alturaMin = p.altura;
            if(p.altura > alturaMax)
                alturaMax = p.altura;
        }
    }
    //PANCHITO 2

    contorno = contornoAlturas.values();
    contornoInt = contornoAlturasInt.values();

    float alturaAniadir = alturaMax - alturaMin;
    alturaAniadir = alturaAniadir / 2.;

    minGlobal = 0;
    maxGlobal = 0;
    foreach(puntoContorno p, contorno){
        puntoInsertando.altura = p.altura-alturaMin-alturaAniadir;
        puntoInsertando.distancia = p.distancia;
        //cout << "altura " << p.altura << "  distancia " << p.distancia << endl;
        if( puntoInsertando.altura < minGlobal)
            minGlobal =  puntoInsertando.altura;
        if( puntoInsertando.altura > maxGlobal){
            maxGlobal =  puntoInsertando.altura;
            distanciaMaxGlobal = puntoInsertando.distancia;
        }
        puntosObtenidos.append(puntoInsertando);
    }
    foreach(puntoContorno p, contornoInt){
        puntoInsertando.altura = p.altura-alturaMin-alturaAniadir;
        puntoInsertando.distancia = p.distancia;
        //cout << "altura " << p.altura << "  distancia " << p.distancia << endl;
        if( puntoInsertando.altura < minGlobal)
            minGlobal =  puntoInsertando.altura;
        if( puntoInsertando.altura > maxGlobal){
            maxGlobal =  puntoInsertando.altura;
            distanciaMaxGlobal = puntoInsertando.distancia;
        }
        puntosObtenidos.append(puntoInsertando);
    }

    GenerarPieza(50);
   // FiltrarPuntos(puntosObtenidos);

    /*
    if(entera)
        GenerarPiezaEntera(50);
    else
        GenerarPieza(50);*/
}

float GLArea::Bezier3(float t, float P1, float P2, float P3){
    return P1*pow((1-t),2) + 2*t*(1-t)*P2 + P3*pow(t,2);
}

void GLArea::GenerarPiezaEntera(int n){
    int numContornoVertices = contornoOrdenado.size() ;
    int numVertices = (numContornoVertices+18)*n+2;
    int numFaces = (numContornoVertices+18)*n*2;
    int abajo1, abajo2;
    float anguloRadianes = 3.1415*2./n;
    float distancia1, distancia1_2, distancia2, distancia2_2;
    float altura1, altura2;
    vcg::Point3f P1,P2,P3;
    int t;
    // puntos del plano
    figuraFinal.Clear();
    vcg::tri::Allocator<CMesh>::AddVertices(figuraFinal,numVertices);
    vcg::tri::Allocator<CMesh>::AddFaces(figuraFinal,numFaces);
    CMesh::VertexPointer ivp[numVertices];
    CMesh::VertexIterator vi=figuraFinal.vert.begin();
    int contador = 0;
    distancia1 = (*contornoOrdenado.begin()).getDistancia();
    altura1 = (*contornoOrdenado.begin()).getAltura();
    distancia1_2 = distancia1 - 0.1;

    distancia2 = contornoOrdenado.values().last().getDistancia();
    altura2 = contornoOrdenado.values().last().getAltura();
    distancia2_2 = distancia2 - 0.1;
    for(int i = 0; i < n; ++i){
        P1.X()=0;P2.Y()=P1.Y()=-1;P1.Z()=0;
        P3.X()=distancia1*cos(anguloRadianes*i);
        P2.X()=distancia1_2*cos(anguloRadianes*i);
        P3.Y()=altura1;
        P3.Z()=distancia1*sin(anguloRadianes*i);
        P2.Z()=distancia1_2*sin(anguloRadianes*i);
        for(t = 1; t< 10; ++t){
            ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (Bezier3(t/10.,P1.X(),P2.X(),P3.X()),
                                                           Bezier3(t/10.,P1.Y(),P2.Y(),P3.Y()),
                                                           Bezier3(t/10.,P1.Z(),P2.Z(),P3.Z()));
            ++vi;++contador;
        }

        foreach(puntoContorno puntito, contornoOrdenado){
            ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (puntito.getDistancia()*cos(anguloRadianes*i),
                                                           puntito.getAltura(),
                                                           puntito.getDistancia()*sin(anguloRadianes*i));
            ++vi;++contador;
        }
        P1.X()=distancia2*cos(anguloRadianes*i);
        P2.X()=distancia2_2*cos(anguloRadianes*i);
        P1.Y()=altura2;
        P1.Z()=distancia2*sin(anguloRadianes*i);
        P2.Z()=distancia2_2*sin(anguloRadianes*i);
        P3.X()=0;P2.Y()=P3.Y()=-0.8;P3.Z()=0;
        for(t = 1; t< 10; ++t){
            ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (Bezier3(t/10.,P1.X(),P2.X(),P3.X()),
                                                           Bezier3(t/10.,P1.Y(),P2.Y(),P3.Y()),
                                                           Bezier3(t/10.,P1.Z(),P2.Z(),P3.Z()));
            ++vi;++contador;
        }
    }
    abajo1 = contador;
    ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (0, -1,  0); ++vi;++contador;
    abajo2 = contador;
    ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (0, -0.8, 0); ++vi;++contador;

    numContornoVertices += 18;

    CMesh::FaceIterator fi=figuraFinal.face.begin();
    for(int i=0; i<n-1; ++i){
        for(int j=0; j<numContornoVertices-1; ++j){
            (*fi).V(0)=ivp[numContornoVertices* i    + j];
            (*fi).V(1)=ivp[numContornoVertices* i    + j+1];
            (*fi).V(2)=ivp[numContornoVertices*(i+1) + j+1];
            ++fi;
            (*fi).V(0)=ivp[numContornoVertices* i    + j];
            (*fi).V(1)=ivp[numContornoVertices*(i+1) + j+1];
            (*fi).V(2)=ivp[numContornoVertices*(i+1) + j];
            ++fi;
        }
        (*fi).V(0)=ivp[abajo1];
        (*fi).V(1)=ivp[numContornoVertices*(i)];
        (*fi).V(2)=ivp[numContornoVertices*(i+1)];
        ++fi;
        (*fi).V(0)=ivp[numContornoVertices*(i+2) - 1];
        (*fi).V(1)=ivp[numContornoVertices*(i+1) - 1];
        (*fi).V(2)=ivp[abajo2];
        ++fi;
    }


    for(int j=0; j<numContornoVertices-1; ++j){
        (*fi).V(0)=ivp[numContornoVertices*(n-1) + j];
        (*fi).V(1)=ivp[numContornoVertices*(n-1) + j+1];
        (*fi).V(2)=ivp[j+1];
        ++fi;
        (*fi).V(0)=ivp[numContornoVertices*(n-1) + j];
        (*fi).V(1)=ivp[j+1];
        (*fi).V(2)=ivp[j];
        ++fi;
    }

    (*fi).V(0)=ivp[abajo1];
    (*fi).V(1)=ivp[numContornoVertices*(n-1)];
    (*fi).V(2)=ivp[0];
    ++fi;
    (*fi).V(0)=ivp[numContornoVertices-1];
    (*fi).V(1)=ivp[numContornoVertices*(n-1) + numContornoVertices-1];
    (*fi).V(2)=ivp[abajo2];
    ++fi;

    // update bounding box
    vcg::tri::UpdateBounding<CMesh>::Box(figuraFinal);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(figuraFinal);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(figuraFinal);
    glFigura.m = &figuraFinal;
    glFigura.Update();
    updateGL();

    emit setStatusBar(tr("Pieza generada"));
}

void GLArea::GenerarPieza(int n){
    int numContornoVertices = puntosObtenidos.size() ;
    int numVertices = (numContornoVertices)*n+2;
    int numFaces = (numContornoVertices)*n*2;
    float anguloRadianes = 3.1415*2./n;
    // puntos del plano
    figuraFinal.Clear();
    vcg::tri::Allocator<CMesh>::AddVertices(figuraFinal,numVertices);
    vcg::tri::Allocator<CMesh>::AddFaces(figuraFinal,numFaces);

    CMesh::VertexPointer ivp[numVertices];
    CMesh::VertexIterator vi=figuraFinal.vert.begin();
    int contador = 0;
    for(int i = 0; i < n; ++i){

        foreach(PuntoContornoLight puntito, puntosObtenidos){
            ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (puntito.distancia*cos(anguloRadianes*i),
                                                           puntito.altura,
                                                           puntito.distancia*sin(anguloRadianes*i));
            ++vi;++contador;
        }
    }


    ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (0, puntosObtenidos.begin()->altura,  0); ++vi;++contador;
    ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (0, (puntosObtenidos.end()-2)->altura, 0); ++vi;++contador;

    CMesh::FaceIterator fi=figuraFinal.face.begin();
    for(int i=0; i<n-1; ++i){
        for(int j=0; j<numContornoVertices-1; ++j){
            //(*(fi)).C() = vcg::Color4b::Green;
            (*fi).V(0)=ivp[numContornoVertices* i    + j];
            (*fi).V(1)=ivp[numContornoVertices* i    + j+1];
            (*fi).V(2)=ivp[numContornoVertices*(i+1) + j+1];
            ++fi;
            //(*(fi)).C() = vcg::Color4b::Green;
            (*fi).V(0)=ivp[numContornoVertices* i    + j];
            (*fi).V(1)=ivp[numContornoVertices*(i+1) + j+1];
            (*fi).V(2)=ivp[numContornoVertices*(i+1) + j];
            ++fi;
        }

       // (*(fi)).C() = vcg::Color4b::Blue;
        (*fi).V(0)=ivp[numContornoVertices*n];
        (*fi).V(1)=ivp[numContornoVertices*(i)];
        (*fi).V(2)=ivp[numContornoVertices*(i+1)];
        ++fi;
      //  (*(fi)).C() = vcg::Color4b::Blue;
        (*fi).V(0)=ivp[numContornoVertices*(i+2) - 1];
        (*fi).V(1)=ivp[numContornoVertices*(i+1) - 1];
        (*fi).V(2)=ivp[numContornoVertices*n + 1];
        ++fi;
    }


    for(int j=0; j<numContornoVertices-1; ++j){
       // (*(fi)).C() = vcg::Color4b::Red;
        (*fi).V(0)=ivp[numContornoVertices*(n-1) + j];
        (*fi).V(1)=ivp[numContornoVertices*(n-1) + j+1];
        (*fi).V(2)=ivp[j+1];
        ++fi;
       // (*(fi)).C() = vcg::Color4b::Red;
        (*fi).V(0)=ivp[numContornoVertices*(n-1) + j];
        (*fi).V(1)=ivp[j+1];
        (*fi).V(2)=ivp[j];
        ++fi;
    }

 //   (*(fi)).C() = vcg::Color4b::Yellow;
    (*fi).V(0)=ivp[numContornoVertices*n];
    (*fi).V(1)=ivp[numContornoVertices*(n-1)];
    (*fi).V(2)=ivp[0];
    ++fi;
//    (*(fi)).C() = vcg::Color4b::Yellow;
    (*fi).V(0)=ivp[numContornoVertices-1];
    (*fi).V(1)=ivp[numContornoVertices*(n-1) + numContornoVertices-1];
    (*fi).V(2)=ivp[numContornoVertices*n + 1];
    ++fi;

    // update bounding box
    vcg::tri::UpdateBounding<CMesh>::Box(figuraFinal);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(figuraFinal);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(figuraFinal);
    glFigura.m = &figuraFinal;
    glFigura.Update();
    updateGL();
}

void GLArea::ResetContorno(){
    for(int i=0; i<numContornos; ++i)
        contornos[i].clear();
    for(int i=0; i<numContornosInt; ++i)
        contornosInt[i].clear();
    numContornos = numContornosInt = 0;
    updateGL();
}


void GLArea::deshacerContorno(){
    if(contornoExterior){
        contornos[numContornos-1].clear();
        numContornos--;
    }
    if(contornoInterior){
        contornosInt[numContornosInt-1].clear();
        numContornosInt--;
    }
    updateGL();
}

void GLArea::clickado(int posx, int posy){
    std::vector<CMesh::FacePointer> resultadoSeleccion;
    puntoContorno punto;
    vcg::Point3f puntoEje, nuevoPunto, incrementoPunto;
    vcg::Line3f ejeY (vcg::Point3f(0,-1000,0),vcg::Point3f(0,1000,0));
    float altura, distancia;
    vcg::GLPickTri<CMesh>::PickFace(posx,posy,meshExt,resultadoSeleccion);
    cout << resultadoSeleccion.size() << "<<<<<<\n";
    for(int i = 0; i < 1 && i < resultadoSeleccion.size(); ++i){
        vcg::Triangle3<float> triang ((*(resultadoSeleccion[i])).P(0),
                                      (*(resultadoSeleccion[i])).P(1),
                                      (*(resultadoSeleccion[i])).P(2));
        nuevoPunto = vcg::Barycenter(triang);
        punto.setPosicion(nuevoPunto);
        puntoEje = vcg::ClosestPoint(ejeY,nuevoPunto);
        incrementoPunto = puntoEje - ejeY.Origin();
        altura = sqrt(pow(incrementoPunto.X(),2)+
                      pow(incrementoPunto.Y(),2)+
                      pow(incrementoPunto.Z(),2));
        altura = incrementoPunto.Norm();
        distancia = vcg::Distance(ejeY,nuevoPunto);
        punto.setAltura(altura);
        punto.setNumPunto(numPuntoContorno);
        punto.setDistancia(distancia);
        if(contornoExterior){
            contornos[numContornos-1].push_back(punto);
        }if(contornoInterior){
            contornosInt[numContornosInt-1].push_back(punto);
        }
        if(altura > puntoAlto.getAltura()){
            puntoAlto.setAltura(altura);
            puntoAlto.setDistancia(distancia);
            puntoAlto.setNumPunto(numPuntoContorno);
            puntoAlto.setPosicion(nuevoPunto);
        }else if(altura < puntoBajo.getAltura()){
            puntoBajo.setAltura(altura);
            puntoBajo.setDistancia(distancia);
            puntoBajo.setNumPunto(numPuntoContorno);
            puntoBajo.setPosicion(nuevoPunto);
        }
        numPuntoContorno++;
    }
    emit ContornoEmpiezaACalcularse();
    updateGL();
}

void GLArea::cambiaValor(int valor){
    cantidadDeIntersecciones=valor;
    if(maxInterseccionVoxel!=0)
    valorColorVoxel = maxInterseccionVoxel - cantidadDeIntersecciones;
    valorColorVoxel = valorColorVoxel / 7;
    if(valorColorVoxel <= 0) valorColorVoxel = 1;
    updateGL();
}

void GLArea::alternarDibujoEje(){
    dibujarEje = !dibujarEje;
    track.Reset();
    updateGL();
}

void GLArea::alternarDibujoVoxels(){
    dibujarVoxels = !dibujarVoxels;
    updateGL();
}

void GLArea::continuarRANSAC(){
    if(ransacCalculado){
        hebra->setOperacion(1,metodoCluster);
        hebra->insertarVoxels(voxels);
        hebra->start();
    }else
        emit setStatusBar(tr("No se puede continuar calculando RANSAC si no se ha iniciado a calcular"));
}

void GLArea::AdaptarEje(){
    vcg::Point3f puntoMin, puntoMax, puntoCandidato, puntoEje;
    float distanciaNeg, distanciaPos, dis;
    distanciaNeg = 10000;
    distanciaPos = -10000;
    CMesh::VertexIterator vi=glWrap.m->vert.begin();
    do{
        puntoCandidato = vcg::Point3f((*vi).P());
        puntoEje = vcg::ClosestPoint(ejeFinal,puntoCandidato);
        puntoCandidato = ejeFinal.Origin() - puntoEje;
        dis = puntoCandidato.Norm();
        if(dis < distanciaNeg){
            distanciaNeg = dis;
            puntoMin = puntoEje;
        }
        if(dis > distanciaPos){
            distanciaPos = dis;
            puntoMax = puntoEje;
        }
        ++vi;
    }while(vi!=glWrap.m->vert.end());
    ejeFinal.Set(puntoMax,(puntoMin-puntoMax));
}

void GLArea::calculoAcabado(QMultiMap<int,Voxel> voxelsFinal, float valorMedioFinal, vcg::Point3f nodoRaizFinal, int maxInterseccionVoxelFinal, vcg::Line3f ejeFinalFinal){
    float calidad;
    voxels = voxelsFinal;
    valorMedio = valorMedioFinal;
    nodoRaiz = nodoRaizFinal;
    maxInterseccionVoxel = maxInterseccionVoxelFinal;
    ejeFinal = ejeFinalFinal;
    AdaptarEje();
    emit ajustarMaximoVoxeles(maxInterseccionVoxel);
    valorColorVoxel = maxInterseccionVoxel - cantidadDeIntersecciones;
    valorColorVoxel = valorColorVoxel / 7;
    if(valorColorVoxel <= 0) valorColorVoxel = 1;

    if(realizarGenetico)
        calidad = calidadDelEje();
    else
        calidad = -1;

    calidadAnterior = calidad;
    emit Imprimir("\nLa calidad del eje: " + QString::number(calidad) + " y el tamanio del mismo " + QString::number(ejeFinal.Direction().Norm()));
    if(calidad>=0){
        dibujarAristas = true;
    }
    noMejora = false;
    estadoDelCalculo = 1;
    dibujarEje = true;
    track.Reset();
    anguloRotar = atan2(puntoOrigen.Y(),puntoOrigen.Z())*180./PI - 180;
    girarMalla();
    EmitirInformacionEje();
    emit activaBotonContorno();
}

void GLArea::cargandoEje(float * valor){
    vcg::Point3f origen(valor[0],valor[1],valor[2]);
    vcg::Point3f direccion(valor[3],valor[4],valor[5]);
    anguloRotar = valor[6];
    anguloPieza = valor[7];
    ejeFinal.Set(origen,direccion);
    girarMalla();
    dibujarEje = true;
    track.Reset();
    estadoDelCalculo = 1;
}

void GLArea::cargandoContorno(float * valor){
    int tamanioValor = valor[0];
    PuntoContornoLight punto;
    for(int i=1; i<tamanioValor; i=i+2){
        punto.distancia = valor[i];
        punto.altura = valor[i+1];
        puntosObtenidos.append(punto);
    }
    verDibujoArqueologico();
    estadoDelCalculo = 2;
}

void GLArea::cargandoFinal(float * valor){
    vcg::Point3f luz3d(valor[0],valor[1],valor[2]);
    vcg::Point3f luzDA(valor[3],valor[4],valor[5]);
    girarMalla();
    dibujarEje = true;
    track.Reset();
    estadoDelCalculo = 3;
}

void GLArea::EmitirInformacionEje(){
    QString texto;
    texto.append(QString::number(ejeFinal.Origin().X()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Origin().Y()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Origin().Z()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Direction().X()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Direction().Y()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Direction().Z()));
    texto.append("\n");
    texto.append(QString::number(anguloRotar));
    texto.append("\n");
    texto.append(QString::number(anguloPieza));
    texto.append("\n");
    emit InformacionEje(texto);
}

void GLArea::EmitirInformacionContorno(){
    QString texto;
    texto.append(QString::number(puntosObtenidos.size()));
    texto.append("\n");
    for(int i=0; i<puntosObtenidos.size(); ++i){
        texto.append(QString::number(puntosObtenidos.at(i).distancia));
        texto.append("\n");
        texto.append(QString::number(puntosObtenidos.at(i).altura));
        texto.append("\n");
}
    emit InformacionContorno(texto);
}

void GLArea::EmitirInformacionFinal(){
    QString texto;
    texto.append(QString::number(ejeFinal.Origin().X()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Origin().Y()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Origin().Z()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Direction().X()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Direction().Y()));
    texto.append("\n");
    texto.append(QString::number(ejeFinal.Direction().Z()));
    texto.append("\n");
    texto.append(QString::number(anguloRotar));
    emit InformacionEje(texto);
}


float GLArea::reCalidadDelEje(int genetico){
    if(genetico == 1){
        if(!noMejora){
            float calidad;
            nuevoCentro[0] = nuevoCentro[0] + (vcg::ClosestPoint(ejeFinal,centro[0])-nuevoCentro[0])/2;
            nuevoCentro[1] = nuevoCentro[1] + (vcg::ClosestPoint(ejeFinal,centro[1])-nuevoCentro[1])/2;
            vcg::Point3f origen = nuevoCentro[0];
            vcg::Point3f destino = nuevoCentro[1]-nuevoCentro[0];
            destino = destino.Normalize();
            float producto = -origen.Z()/destino.Z();
            origen = origen-destino*producto;
            ejeFinal.Set(origen,destino);
            calidad = calidadDelEje();
            if(calidad > calidadAnterior){
                    ejeFinal.Set(ejeDePrueba.Origin(),ejeDePrueba.Direction());
                    noMejora = true;
                    emit setStatusBar(tr("Ya no puede seguir calculando, no mejora."));
            }else{
                    calidadAnterior = calidad;
            }
            emit Imprimir("\nLa calidad del eje: " + QString::number(calidad));
        }else
            emit setStatusBar(tr("¡Que no mejora, he dicho!"));
    }else{
        calidadDelEje();
        dibujarAristas = true;
    }
}

void GLArea::ObtenerPlanosCorte(){
    vcg::Plane3f plano;
    float Distancia;
    float altura = -glWrap.m->bbox.MaxDim();
    do{
        plano.Set(ejeFinal.Direction(),altura);
        altura = altura + 0.001;
        //cout << "\n" << altura << "\n";
    }while(!InterseccionPlanoPieza(plano,0));
    Distancia = altura;
    altura += 0.002;
    plano.Set(ejeFinal.Direction(),altura);
    InterseccionPlanoPieza(plano,0);
    altura = glWrap.m->bbox.MaxDim();
    emit AvanzarBarra((int) 20);
    do{
        plano.Set(ejeFinal.Direction(),altura);
        altura = altura - 0.001;
        //cout << "\n" << altura << "\n";
    }while(!InterseccionPlanoPieza(plano,1));
    altura = altura - 0.002;
    Distancia = Distancia - altura;
    Distancia = Distancia / 8.;
    altura = Distancia*3;
    plano.Set(ejeFinal.Direction(),altura);
    InterseccionPlanoPieza(plano,0);
    altura = -altura;
    plano.Set(ejeFinal.Direction(),altura);
    InterseccionPlanoPieza(plano,1);
}

bool GLArea::InterseccionPlanoPieza(vcg::Plane3f plano, int planoActual){

    if(carasSeleccionadas.isEmpty()){
        vcg::Intersection<CMesh,CEMesh, CMesh::ScalarType>(mesh,plano,aristacas[planoActual]);
    }else{
        CMesh::VertexIterator vi, vi2;
        vcg::Segment3<CMesh::ScalarType> seg;
        foreach(CMesh::FacePointer cara, carasSeleccionadas){
            if(vcg::IntersectionPlaneTriangle(plano,*cara,seg)){
                vcg::edg::Allocator<CEMesh>::AddEdges(aristacas[planoActual],1);
                vi2 = vcg::edg::Allocator<CEMesh>::AddVertices(aristacas[planoActual],2);
                (*vi2).P() = seg.P0();
                aristacas[planoActual].edges.back().V(0)=&(*vi2);
                ++vi2;
                (*vi2).P() = seg.P1();
                aristacas[planoActual].edges.back().V(0)=&(*vi2);
            }
        }
    }
    return (aristacas[planoActual].edges.size() > 0);
}

float GLArea::calidadDelEje(){
    emit IniciarBarra();
    QMultiMap<float,vcg::Point3f> puntosOrdenadosDistancia;
    QMultiMap<float,vcg::Point3f>::iterator it;
    ejeDePrueba.Set(ejeFinal.Origin(),ejeFinal.Direction());
    numPlanos = 2;
    vcg::Point3f puntoNuevo, puntoViejo, ejeRotacion;
    delete [] aristacas;
    delete [] centro;
    delete [] puntosDibujar;
    delete [] nuevoCentro;
    aristacas = new CEMesh [numPlanos];
    centro = new vcg::Point3f [numPlanos];
    nuevoCentro = new vcg::Point3f [numPlanos];
    puntosDibujar = new QList<vcg::Point3f> [numPlanos];
    QList<vcg::Point3f> puntosDibujarG;
    CEMesh::EdgeIterator ei;
    vcg::Quaternionf quaternion, quaternion2, quaternionInverso, quaternionInverso2;
    vcg::Matrix33f matrizGrande;
    vcg::Point3f vectorDerecha,vectorIzquierda;
    float a, b, theta, errorAcumulado = 0, * resultado;

    ObtenerPlanosCorte();


    emit AvanzarBarra((int)(40));

    vcg::Point3f ejeX (1,0,0), aux;
    ejeRotacion = ejeFinal.Direction()^ejeX;
    vcg::Line3f lineaX;
    lineaX.Set(vcg::Point3f(0,0,0),ejeX);
    a = (ejeFinal.Direction() - vcg::ClosestPoint(lineaX,ejeFinal.Direction())).Norm();
    b = ejeFinal.Direction().X();
    theta = atan2(a,b);
    quaternion.FromAxis(theta,ejeRotacion);
    quaternionInverso.FromAxis(-theta,ejeRotacion);

    puntoOrigen = quaternion.Rotate(ejeFinal.Origin());
    aux = quaternion.Rotate(ejeFinal.Direction());
    //dibujarEjeDePrueba = true;
    int contar = 0;
    for(int i=0; i<numPlanos; ++i){
        emit AvanzarBarra((int)(40+30.*i));
        ei = aristacas[i].edges.begin();
        puntosOrdenadosDistancia.clear();
        while(ei!=aristacas[i].edges.end()){
            puntoNuevo = ei->P(0);
            ei++;
            puntosOrdenadosDistancia.insert((puntoNuevo - vcg::ClosestPoint(ejeFinal,puntoNuevo)).Norm(),puntoNuevo);
        }
        contar = (int) puntosOrdenadosDistancia.size()/2. -1;
        it = puntosOrdenadosDistancia.begin();
        while(contar > 0){
            it = puntosOrdenadosDistancia.erase(it);
            contar--;
        }
        puntosDibujar[i] = puntosOrdenadosDistancia.values();
        puntosDibujarG.clear();
        foreach(vcg::Point3f punto, puntosDibujar[i]){
            puntoNuevo = quaternion.Rotate(punto);
            puntosDibujarG.append(puntoNuevo);
        }

        resultado = ajusteCirculo(puntosDibujarG);

        centro[i].X() = puntoNuevo.X();
        centro[i].Z() = resultado[0];
        centro[i].Y() = resultado[1];
        centro[i] = quaternionInverso.Rotate(centro[i]);
        nuevoCentro[i] = vcg::ClosestPoint(ejeFinal,centro[i]);
        errorAcumulado = errorAcumulado + 1.*(nuevoCentro[i]-centro[i]).Norm();
        nuevoCentro[i] = centro[i];
    }
    emit AcabarBarra();
    emit setStatusBar(tr("Calidad calculada."));
    return 1.*errorAcumulado;
}

float * GLArea::ajusteCirculo(QList<vcg::Point3f> conjuntoPuntos){
        float xm, ym, factor, error, a0, b0, Li, La, Lb, L, a, b, r[3];
        xm = 0;
        ym = 0;
        factor = 1.0 / float(conjuntoPuntos.size());
        foreach(vcg::Point3f puntoActual, conjuntoPuntos){
                xm += puntoActual.Z();
                ym += puntoActual.Y();
        }
        xm *= factor;
        ym *= factor;

        a0 = xm;
        b0 = ym;

        do{
            La=Lb=L=0;
            foreach(vcg::Point3f puntoActual, conjuntoPuntos){
                Li = sqrt((puntoActual.Z()-a0)*(puntoActual.Z()-a0)+(puntoActual.Y()-b0)*(puntoActual.Y()-b0));
                La+=(a0-puntoActual.Z())/Li;
                Lb+=(b0-puntoActual.Y())/Li;
                L+=Li;
            }
            L*=factor;
            La*=factor;
            Lb*=factor;
            a = xm+L*La;
            b = ym+L*Lb;
            error = sqrt((a0-a)*(a0-a)+(b0-b)*(b0-b));
            a0 = a;
            b0 = b;
        }while(error > 0.00001);

        Li=0;
        foreach(vcg::Point3f puntoActual, conjuntoPuntos)
                Li+=sqrt((puntoActual.Z()-a0)*(puntoActual.Z()-a0)+(puntoActual.Y()-b0)*(puntoActual.Y()-b0));
        r[2] = Li*factor;
        r[0] = a;
        r[1] = b;

        return r;
}

void GLArea::MoverPieza(bool valor){
    moverPieza = valor;
}

void GLArea::FiltrarPuntos(QList<PuntoContornoLight> &puntos){
    /*int numSaltos = 10,mediana, mitad, mitadOffset;
    float alturaMin = 1000000, alturaMax=-1000000, saltoAltura, altura;
    PuntoContornoLight punto1, punto2;
    QMap<float,PuntoContornoLight> puntosDistancias;
    QList<PuntoContornoLight>::iterator puntosIt;
    QList<PuntoContornoLight> puntosFuera, puntosDentro;


    foreach(PuntoContornoLight punto, puntos){
        altura = punto.altura;
        if(altura < alturaMin)
            alturaMin = altura;
        else if(altura > alturaMax)
            alturaMax = altura;
    }
    saltoAltura = (alturaMax - alturaMin)/numSaltos;

    puntosIt = puntos.begin();
    for(altura = alturaMin+saltoAltura; altura < alturaMax; altura+=saltoAltura){
        puntosDistancias.clear();
        cout << "\n Altura=> " << altura;
        while(puntosIt->altura < altura){
            puntosDistancias.insertMulti(puntosIt->distancia,(*puntosIt));
            puntosIt++;
        }
        cout << "\n" << puntosDistancias.size();
        if(puntosDistancias.size() == 1){
            puntosFuera.append(puntosDistancias.values().at(0));
        }else if(puntosDistancias.size() > 1){
            mitadOffset = mitad = puntosDistancias.size()/2;
            if(mitad%2!=0){
                puntosDentro.prepend(puntosDistancias.values().at((mitad+1)/2));
            }else{
                punto1 = puntosDistancias.values().at(mitad/2);
                punto2 = puntosDistancias.values().at(1 + mitad/2);
                punto1.distancia = punto1.distancia+punto2.distancia /2.;
                punto1.altura = punto1.altura+punto2.altura /2.;
                puntosDentro.prepend(punto1);
            }
            mitad = puntosDistancias.size() - mitadOffset;
            if(mitad%2!=0){
                puntosFuera.append(puntosDistancias.values().at(mitadOffset + (mitad+1)/2));
            }else{
                punto1 = puntosDistancias.values().at(mitadOffset + mitad/2);
                punto2 = puntosDistancias.values().at(mitadOffset + 1 + mitad/2);
                punto1.distancia = punto1.distancia+punto2.distancia /2.;
                punto1.altura = punto1.altura+punto2.altura /2.;
                puntosFuera.append(punto1);
            }
        }*/
   // }
        /*
    puntos.clear();
    puntos.append(puntosFuera);
    puntos.append(puntosDentro);*/
}

void GLArea::ComprobarContornos(){
    if(numContornos == MAXCONTORNOS-1){
        QList<puntoContorno> * aux;
        MAXCONTORNOS+=100;
        aux = new QList<puntoContorno> [MAXCONTORNOS];
        for(int i=0; i<numContornos; ++i){
            aux[i].append(contornos[i]);
            contornos[i].clear();
        }
        delete [] contornos;
        contornos = aux;
    }
    if(numContornosInt == MAXCONTORNOSINT-1){
        QList<puntoContorno> * aux;
        MAXCONTORNOSINT+=100;
        aux = new QList<puntoContorno> [MAXCONTORNOSINT];
        for(int i=0; i<numContornosInt; ++i){
            aux[i].append(contornosInt[i]);
            contornosInt[i].clear();
        }
        delete [] contornosInt;
        contornosInt = aux;
    }
}

void GLArea::ExportarImagen(QIODevice * salida, const char * formato){
    // Make GL Context current
    makeCurrent();
         // Copy from OpenGL
    QImage *tempImage = new QImage( grabFrameBuffer() );
    if ( ! tempImage );
    if ( !tempImage->save( salida, formato) );
         // Cleaning memory
    delete tempImage;
    salida->close();
}
