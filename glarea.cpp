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


    ResetearPieza();

    updateGL();

}

void GLArea::ResetearPieza(){
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
    dibujarEje = dibujarEjeDePrueba = dibujarAristas = dibujarVoxels = piezaCargada = ransacCalculado = cambiaDibujo = pintar = planoInterseccion = ejeDibujando = normalesObtenidas = voxelesDibujando = seleccionarActivado = false;
    modoDibujado = true;
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
       int err=vcg::tri::io::ExporterPLY<CMesh>::Save(mesh,(fileName.toStdString()).c_str());
            if(err!=0){
              const char* errmsg=vcg::tri::io::ExporterPLY<CMesh>::ErrorMsg(err);
              QMessageBox::warning(this,tr("Error exportando el modelo PLY"),QString(errmsg));
            }
            initMesh("Exportado \""+fileName+"\".");
    }else{
        int err=vcg::tri::io::Exporter<CMesh>::Save(mesh,(fileName.toStdString()).c_str());
        if(err!=0){
        }
            initMesh("Exportado \""+fileName+"\".");
    }
}

void GLArea::cancelarSalir()
{

}

void GLArea::loadTetrahedron(){
    /*
    vcg::tri::Tetrahedron(mesh);
    dibujarNormales = normalesObtenidas = ejeDibujando = planoInterseccion = false;
    initMesh(tr("Tetaedro [builtin]"));*/
}

void GLArea::dibujaPlano(){
    planoInterseccion = !planoInterseccion;
    if(planoInterseccion = true) cargarPlano();
}

void GLArea::cargarPlano(){
    // puntos del plano
    plano.Clear();
    vcg::tri::Allocator<CMesh>::AddVertices(plano,3);
    vcg::tri::Allocator<CMesh>::AddFaces(plano,2);

    CMesh::VertexPointer ivp[3];
    CMesh::VertexIterator vi=plano.vert.begin();
    ivp[0]=&*vi;(*vi).P()=CMesh::CoordType ( 10.0, 0.0, 10.0); ++vi;
    ivp[1]=&*vi;(*vi).P()=CMesh::CoordType (  0.0, 0.0,-10.0); ++vi;
    ivp[2]=&*vi;(*vi).P()=CMesh::CoordType (-10.0, 0.0, 10.0); ++vi;

    CMesh::FaceIterator fi=plano.face.begin();
    (*fi).V(0)=ivp[0];  (*fi).V(1)=ivp[1]; (*fi).V(2)=ivp[2]; ++fi;
    (*fi).V(0)=ivp[2];  (*fi).V(1)=ivp[1]; (*fi).V(2)=ivp[0];

    // update bounding box
    vcg::tri::UpdateBounding<CMesh>::Box(plano);
    // update Normals
    vcg::tri::UpdateNormals<CMesh>::PerVertexNormalizedPerFace(plano);
    vcg::tri::UpdateNormals<CMesh>::PerFaceNormalized(plano);
    glWrapPlano.m = &plano;
    glWrapPlano.Update();
    updateGL();
    
    emit setStatusBar(tr("Plano cargado"));
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

    updateGL();
    emit setTextoInformacion(QString::number(glWrap.m->face.size())+" caras. "+
                             QString::number(glWrap.m->vert.size())+" vertices.");
    emit setStatusBar(message);
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

void GLArea::paintGL ()
{
    if(pintar){
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
            if(modoDibujado) //¿Pieza reconstruida?
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

            glPushMatrix();
            //Mover para que la pieza reconstruida quede centrada en el eje
            glRotatef(90,1,0,0);
            glTranslatef(0,-ejeFinal.Origin().Y(),0);
            
            //Dibujar el contorno cutre
            glDisable(GL_LIGHTING);
                glPointSize(3);
            glColor3f(1,0.4,0.4);
            glBegin(GL_POINTS);
                    glVertex3f(puntosObtenidos.at(0).distancia*cos(0),
                               puntosObtenidos.at(0).altura,
                               puntosObtenidos.at(0).distancia*sin(0));
            glEnd();
            glColor3f(0.4,1,0.4);
            glBegin(GL_POINTS);
                for(int i=1; i<puntosObtenidos.size(); ++i){
                    glVertex3f(puntosObtenidos.at(i).distancia*cos(0),
                               puntosObtenidos.at(i).altura,
                               puntosObtenidos.at(i).distancia*sin(0));
                }
            glEnd();
            glColor3f(0.4,1,0.4);
            glBegin(GL_LINE_STRIP);
                foreach(PuntoContornoLight puntoActual, puntosObtenidos){
                    glVertex3f(puntoActual.distancia*cos(0),
                               puntoActual.altura,
                               puntoActual.distancia*sin(0));
                }
            glEnd();
            glColor3f(0.4,1,0.4);
            glBegin(GL_LINE_STRIP);
                foreach(PuntoContornoLight puntoActual, puntosGrahamOrdenados){
                    glVertex3f(puntoActual.distancia*cos(0),
                               1 + puntoActual.altura,
                               puntoActual.distancia*sin(0));
                }
            glEnd();
            glColor3f(0.4,0.4,1);
            glBegin(GL_LINES);
                foreach(PuntoContornoLight puntoActual, puntosObtenidos){
                    glVertex3f(puntosObtenidos.at(0).distancia*cos(0),
                               puntosObtenidos.at(0).altura,
                               2 + puntosObtenidos.at(0).distancia*sin(0));
                    glVertex3f(puntoActual.distancia*cos(0),
                               puntoActual.altura,
                               2 + puntoActual.distancia*sin(0));
                }
            glEnd();
            glEnable(GL_LIGHTING);
            glPopMatrix();
        }else{ //Dibujar ANTES de reconstruir
            if(planoInterseccion) //Dibujar plano de prueba
                glWrapPlano.Draw<vcg::GLW::DMFlat, vcg::GLW::CMPerFace, vcg::GLW::TMNone> ();
            if(dibujarEje){
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
                }
                glEnable(GL_LIGHTING);
                glPopMatrix();
            }
            if(dibujarVoxels){ //Dibujar vóxeles
                if(voxelesDibujando){
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
                        //                glWrapEje.Draw<vcg::GLW::DMFlatWire, vcg::GLW::CMNone, vcg::GLW::TMNone> ();
                }
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
            glColor3f(0,1,0);
            glBegin(GL_LINE_STRIP);
            foreach(puntoContorno puntoActual, contorno){ //Dibuja el contorno seleccionado
                glVertex3f(puntoActual.getPosicion().X(),puntoActual.getPosicion().Y(),puntoActual.getPosicion().Z());
            }
            glEnd();
            glPopMatrix();
            if(dibujarAristas){
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
            if(modoDibujado)
                switch(drawmode)
                { //Dibuja pieza sin reconstruir
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
  updateGL ();
}

void GLArea::mousePressEvent (QMouseEvent * e)
{
  e->accept ();
  setFocus ();
  if(seleccionarActivado){
        if(e->buttons()&Qt::RightButton){
            track.MouseDown (e->x (), height () - e->y (), QT2VCG (Qt::LeftButton, e->modifiers ()));
            botonIzquierdo = false;
        }else{
            clickado(e->x(), height() - e->y());
            botonIzquierdo = true;
        }
  }else
        track.MouseDown (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));

  updateGL ();
}

void GLArea::mouseMoveEvent (QMouseEvent * e)
{
  if (e->buttons ()) {
      if(seleccionarActivado)
        if(botonIzquierdo)
            clickado(e->x(), height() - e->y());
        else
            track.MouseMove(e->x (), height () - e->y ());
      else
        track.MouseMove (e->x (), height () - e->y ());
    updateGL ();
    }else{
        emit RatonEnGLArea();
    }
}

void GLArea::mouseReleaseEvent (QMouseEvent * e)
{

  if(seleccionarActivado)
    if(botonIzquierdo)
        clickado(e->x(), height() - e->y());
    else
        track.MouseUp (e->x (), height () - e->y (), QT2VCG (Qt::LeftButton, e->modifiers ()));
  else
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
    parametros[1] = 40;
    parametros[2] = 90;
    parametros[3] = 50;
    parametros[4] = 1000;
    parametros[5] = 10;
    parametros[6] = 0;
    emit CrearConfiguracionEje(parametros);
}

void GLArea::inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje, bool refinarEje){
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
            hebra->setOperacion(0);
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

void GLArea::CalcularContorno(bool entera){

    int numPuntos, numPuntos2, numElementos;
    float alturaAlta = puntoAlto.getAltura(), alturaBaja = puntoBajo.getAltura(), saltoAltura = (alturaAlta - alturaBaja)/10., altura;
    float distanciaMin, distanciaMax, distancia;
    puntoContorno punto1, punto2;
    vcg::Point3f puntoEje = vcg::ClosestPoint(ejeFinal,puntoAlto.getPosicion());
    contornoOrdenado.clear();
    QMap<float, PuntoContornoLight> puntosLaterales;
    PuntoContornoLight puntoInsertando;
    QList<PuntoContornoLight> listaLaterales;
    QList<float> listaAlturas;
    QMap<float, float> puntosAlturas;
    QMap<float, puntoContorno>::iterator itAlturas;
    QList<PuntoContornoLight>::iterator itPuntos;
    puntoContorno puntoNuevo;
    int contandito = 0, contadorPuntos = 0;
    foreach(puntoContorno punto, contornoAlturas){
        puntoInsertando.altura = punto.altura - alturaBaja;
        puntoInsertando.distancia = punto.distancia;
        puntosObtenidos.append(puntoInsertando);
       // emit Imprimir("\n Altura: "+QString::number(puntoInsertando.altura));
    }
    alturaAlta -= alturaBaja;
    alturaBaja = 0;
    puntosGrahamOrdenados = puntosObtenidos;
    GrahamScan(puntosObtenidos);

    //emit Imprimir("\n AlturaBaja: "+QString::number(alturaBaja) + " AlturaAlta: "+QString::number(alturaAlta)+ " SaltoAltura: "+QString::number(saltoAltura));
/*
    emit Imprimir("\n Imprimiendo valores");

    foreach(PuntoContornoLight punto, puntosObtenidos)
        emit Imprimir("\n" + QString::number(punto.altura) + "\t" + QString::number(punto.distancia));


    GrahamScan(puntosObtenidos);

    emit Imprimir("\n Imprimiendo valores");

    foreach(PuntoContornoLight punto, puntosObtenidos)
        emit Imprimir("\n" + QString::number(punto.altura) + "\t" + QString::number(punto.distancia));


    QList<PuntoContornoLight> puntos;
    PuntoContornoLight p0;
    puntos.append(puntosObtenidos);
    int m = 1;
    int i = 2;
    while(i<puntos.size()){
        emit Imprimir("\nPuntos: " + QString::number(m-1) + " " + QString::number(m) + " " + QString::number(i) +
                      "\n" + QString::number(puntos[m-1].altura) + " " + QString::number(puntos[m-1].distancia) +
                      "\n" + QString::number(puntos[m].altura) + " " + QString::number(puntos[m].distancia) +
                      "\n" + QString::number(puntos[i].altura) + " " + QString::number(puntos[i].distancia) +
                      " " + QString::number(DireccionProductoVectorial(puntos[m-1],puntos[m],puntos[i])));
        while(m >= 1 && DireccionProductoVectorial(puntos[m-1],puntos[m],puntos[i]) < 0){
        emit Imprimir("\nPuntos: " + QString::number(m-1) + " " + QString::number(m) + " " + QString::number(i) +
                      "\n" + QString::number(puntos[m-1].altura) + " " + QString::number(puntos[m-1].distancia) +
                      "\n" + QString::number(puntos[m].altura) + " " + QString::number(puntos[m].distancia) +
                      "\n" + QString::number(puntos[i].altura) + " " + QString::number(puntos[i].distancia) +
                      " " + QString::number(DireccionProductoVectorial(puntos[m-1],puntos[m],puntos[i])));
            m--;
        }
        emit Imprimir("\nPuntos: " + QString::number(m-1) + " " + QString::number(m) + " " + QString::number(i) +
                      "\n" + QString::number(puntos[m-1].altura) + " " + QString::number(puntos[m-1].distancia) +
                      "\n" + QString::number(puntos[m].altura) + " " + QString::number(puntos[m].distancia) +
                      "\n" + QString::number(puntos[i].altura) + " " + QString::number(puntos[i].distancia) +
                      " " + QString::number(DireccionProductoVectorial(puntos[m-1],puntos[m],puntos[i])));
        m++;
        p0 = puntos[m];
        puntos.replace(m,puntos[i]);
        puntos.replace(i,p0);
        i++;
    }

    puntosObtenidos.clear();
    for(int i=0; i<m; ++i)
        puntosObtenidos.append(puntos.at(i));

    emit Imprimir ("\nTamanio de esta cosa\n" + QString::number(puntosGrahamOrdenados.size()) + "\n");

/*
    for(float altura = alturaBaja; altura <= alturaAlta; altura+=saltoAltura){
        puntosLaterales.clear();
        contandito++;
        itAlturas = contornoAlturas.begin();
        distanciaMin = 100000;
        distanciaMax = -1;
        puntosLaterales.clear();
        numElementos = 0;
        while(itAlturas->getAltura() < altura){
            distancia = itAlturas->getDistancia();
            altura = itAlturas->getAltura();
            if(distancia < distanciaMin)
                distanciaMin = distancia;
            if (distancia > distanciaMax)
                distanciaMax = distancia;
            puntoInsertando.altura = altura;
            puntoInsertando.distancia = distancia;
            puntosLaterales.insert(distancia,puntoInsertando);
            itAlturas = contornoAlturas.erase(itAlturas);
            numElementos ++;
        }
            emit Imprimir("Num elementos: " + QString::number(numElementos) + "\n");
            listaLaterales = puntosLaterales.values();
        if(numElementos == 1){
            puntosGrahamOrdenados.append(puntoInsertando);
        }else if(numElementos == 2){
            puntosGrahamOrdenados.append(listaLaterales);
        }else if(numElementos > 2){
            distancia = distanciaMin + (distanciaMax-distanciaMin)/2.;
            foreach(PuntoContornoLight punto, listaLaterales){
                emit Imprimir("\n" + QString::number(punto.distancia));
            }
            numPuntos = 0;
            puntosAlturas.clear();
            for(numPuntos = 0; listaLaterales.at(numPuntos).distancia < distancia && numPuntos < listaLaterales.size();++numPuntos){
                puntosAlturas.insertMulti(listaLaterales.at(numPuntos).altura ,listaLaterales.at(numPuntos).altura);
            }
            listaAlturas = puntosAlturas.values();
            puntosAlturas.clear();
            puntoInsertando.distancia = listaLaterales.at((int) numPuntos/2).distancia;
            puntoInsertando.altura = listaLaterales.at((int) numPuntos/2).altura;
            puntosGrahamOrdenados.append(puntoInsertando);
            for(numPuntos2 = 0; numPuntos2 < numPuntos; numPuntos2++){
                puntosAlturas.insert(listaLaterales.at(numPuntos + numPuntos2).altura, listaLaterales.at(numPuntos + numPuntos2).altura);
            }
            listaAlturas = puntosAlturas.values();
            puntoInsertando.distancia = listaAlturas.at(numPuntos + (int) numPuntos2/2);
            puntoInsertando.altura = listaAlturas.at((int) numPuntos2/2);
            puntosGrahamOrdenados.append(puntoInsertando);
        }
    }
*/
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
    int numContornoVertices = contornoOrdenado.size() ;
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

        foreach(puntoContorno puntito, contornoOrdenado){
            ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (puntito.getDistancia()*cos(anguloRadianes*i),
                                                           puntito.getAltura(),
                                                           puntito.getDistancia()*sin(anguloRadianes*i));
            ++vi;++contador;
        }
    }


    ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (0, contornoOrdenado.begin()->getAltura(),  0); ++vi;++contador;
    ivp[contador]=&*vi;(*vi).P()=CMesh::CoordType (0, (contornoOrdenado.end()-2)->getAltura(), 0); ++vi;++contador;

    CMesh::FaceIterator fi=figuraFinal.face.begin();
    for(int i=0; i<n-1; ++i){
        for(int j=0; j<numContornoVertices-1; ++j){
            (*(fi)).C() = vcg::Color4b::Green;
            (*fi).V(0)=ivp[numContornoVertices* i    + j];
            (*fi).V(1)=ivp[numContornoVertices* i    + j+1];
            (*fi).V(2)=ivp[numContornoVertices*(i+1) + j+1];
            ++fi;
            (*(fi)).C() = vcg::Color4b::Green;
            (*fi).V(0)=ivp[numContornoVertices* i    + j];
            (*fi).V(1)=ivp[numContornoVertices*(i+1) + j+1];
            (*fi).V(2)=ivp[numContornoVertices*(i+1) + j];
            ++fi;
        }

        (*(fi)).C() = vcg::Color4b::Blue;
        (*fi).V(0)=ivp[numContornoVertices*n];
        (*fi).V(1)=ivp[numContornoVertices*(i)];
        (*fi).V(2)=ivp[numContornoVertices*(i+1)];
        ++fi;
        (*(fi)).C() = vcg::Color4b::Blue;
        (*fi).V(0)=ivp[numContornoVertices*(i+2) - 1];
        (*fi).V(1)=ivp[numContornoVertices*(i+1) - 1];
        (*fi).V(2)=ivp[numContornoVertices*n + 1];
        ++fi;
    }


    for(int j=0; j<numContornoVertices-1; ++j){
        (*(fi)).C() = vcg::Color4b::Red;
        (*fi).V(0)=ivp[numContornoVertices*(n-1) + j];
        (*fi).V(1)=ivp[numContornoVertices*(n-1) + j+1];
        (*fi).V(2)=ivp[j+1];
        ++fi;
        (*(fi)).C() = vcg::Color4b::Red;
        (*fi).V(0)=ivp[numContornoVertices*(n-1) + j];
        (*fi).V(1)=ivp[j+1];
        (*fi).V(2)=ivp[j];
        ++fi;
    }

    (*(fi)).C() = vcg::Color4b::Yellow;
    (*fi).V(0)=ivp[numContornoVertices*n];
    (*fi).V(1)=ivp[numContornoVertices*(n-1)];
    (*fi).V(2)=ivp[0];
    ++fi;
    (*(fi)).C() = vcg::Color4b::Yellow;
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

    emit setStatusBar(tr("Pieza generada"));
}

void GLArea::clickado(int posx, int posy){
    std::vector<CMesh::FacePointer> resultadoSeleccion;
 /*   if(modoEje == -1){
        vcg::GLPickTri<CMesh>::PickFace(posx,posy,mesh,resultadoSeleccion);
        for(int i = 0; i < resultadoSeleccion.size(); ++i){
            (*(resultadoSeleccion[i])).C() = vcg::Color4b::Blue;
            if(!carasSeleccionadas.contains(resultadoSeleccion[i]))
                carasSeleccionadas.append(resultadoSeleccion[i]);
        }
    }else{*/
        puntoContorno punto;
        vcg::Point3f puntoEje, nuevoPunto, incrementoPunto;
        float altura, distancia;
        vcg::GLPickTri<CMesh>::PickFace(posx,posy,mesh,resultadoSeleccion);
        for(int i = 0; i < 1 && i < resultadoSeleccion.size(); ++i){
            (*(resultadoSeleccion[i])).C() = vcg::Color4b::Red;
            vcg::Triangle3<float> triang ((*(resultadoSeleccion[i])).P(0),
                                          (*(resultadoSeleccion[i])).P(1),
                                          (*(resultadoSeleccion[i])).P(2));
            nuevoPunto = vcg::Barycenter(triang);
            punto.setPosicion(nuevoPunto);
            puntoEje = vcg::ClosestPoint(ejeFinal,nuevoPunto);
            incrementoPunto = puntoEje - ejeFinal.Origin();
            altura = sqrt(pow(incrementoPunto.X(),2)+
                          pow(incrementoPunto.Y(),2)+
                          pow(incrementoPunto.Z(),2));
            altura = incrementoPunto.Norm();
            distancia = vcg::Distance(ejeFinal,nuevoPunto);
            punto.setAltura(altura);
            punto.setNumPunto(numPuntoContorno);
            punto.setDistancia(distancia);
            contornoAlturas.insert(altura,punto);
            contorno.push_back(punto);
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
    //}
    updateGL();
}

float GLArea::Angulo(vcg::Point3f punto1, vcg::Point3f punto2){
    vcg::Point3f punto_1, punto_2;
    vcg::Plane3f plano (0,ejeFinal.Direction());
    punto_1 = plano.Projection(punto1);
    punto_2 = plano.Projection(punto2);
    float angulo = acosf((punto_1.X()*punto_2.X()+
                  punto_1.Y()*punto_2.Y())
                 /
                 (sqrt(pow(punto_1.X(),2)+pow(punto_1.Y(),2))*
                  sqrt(pow(punto_2.X(),2)+pow(punto_2.Y(),2))));
    if(punto_2.X() < punto_1.X())
        return angulo;
    else
        return -angulo;
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
    updateGL();
}

void GLArea::alternarDibujoVoxels(){
    dibujarVoxels = !dibujarVoxels;
    updateGL();
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

void GLArea::continuarRANSAC(){
    if(ransacCalculado){
        hebra->setOperacion(1);
        hebra->insertarVoxels(voxels);
        hebra->start();
    }else
        emit setStatusBar(tr("No se puede continuar calculando RANSAC si no se ha iniciado a calcular"));
}

void GLArea::calculoAcabado(QMultiMap<int,Voxel> voxelsFinal, float valorMedioFinal, vcg::Point3f nodoRaizFinal, int maxInterseccionVoxelFinal, vcg::Line3f ejeFinalFinal){
    float calidad;
    voxels = voxelsFinal;
    dibujarEje = parametrosCargados[8];
    if(dibujarEje) emit activaBotonEje();
    if(calcularV) emit activaBotonVoxels();
    voxelesDibujando = true;
    valorMedio = valorMedioFinal;
    nodoRaiz = nodoRaizFinal;
    maxInterseccionVoxel = maxInterseccionVoxelFinal;
    ejeFinal = ejeFinalFinal;
    while(ejeFinal.Direction().Norm() < 1){
        ejeFinal.Direction()*10;
    }
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
}

void GLArea::GeneticoRefinaEje(){
    bool reiterar = true;
    vcg::Line3f ejeCandidato;
    vcg::Line3f ejeActual;
    vcg::Point3f origen = nuevoCentro[0];
    vcg::Point3f destino = nuevoCentro[1]-nuevoCentro[0];
    vcg::Point3f nuevoCentro0Actual = nuevoCentro[0];
    vcg::Point3f nuevoCentro1Actual = nuevoCentro[1];
    float calidadMinima = calidadDelEje(), calidadCandidata, aux;
    while(reiterar){
        reiterar = false;
        ejeActual = ejeFinal;
        nuevoCentro0Actual = nuevoCentro[0];
        nuevoCentro1Actual = nuevoCentro[1];
        origen = nuevoCentro[0];
        destino = (nuevoCentro[1]-nuevoCentro[0]).Normalize();
        aux = -origen.Z()/destino.Z();
        origen = origen-destino*aux;
        ejeFinal.Set(origen,destino);
        calidadCandidata = calidadDelEje();
        if(calidadCandidata < calidadMinima){
            calidadMinima = calidadCandidata;
            ejeCandidato.Set(origen,destino);
            reiterar = true;
        }
        origen = nuevoCentro0Actual + (vcg::ClosestPoint(ejeActual,nuevoCentro0Actual)-nuevoCentro0Actual)/2;
        destino = nuevoCentro0Actual + (vcg::ClosestPoint(ejeActual,nuevoCentro0Actual)-nuevoCentro0Actual)/2;
        destino = destino - origen;
        destino = destino.Normalize();
        aux = -origen.Z()/destino.Z();
        origen = origen-destino*aux;
        ejeFinal.Set(origen,destino);
        calidadCandidata = calidadDelEje();
        if(calidadCandidata < calidadMinima){
            calidadMinima = calidadCandidata;
            ejeCandidato.Set(origen,destino);
            reiterar = true;
        }
        origen = nuevoCentro0Actual + (vcg::ClosestPoint(ejeActual,nuevoCentro0Actual)-nuevoCentro0Actual)*2;
        destino = nuevoCentro0Actual + (vcg::ClosestPoint(ejeActual,nuevoCentro0Actual)-nuevoCentro0Actual)*2;
        destino = destino - origen;
        destino = destino.Normalize();
        aux = -origen.Z()/destino.Z();
        origen = origen-destino*aux;
        ejeFinal.Set(origen,destino);
        calidadCandidata = calidadDelEje();
        if(calidadCandidata < calidadMinima){
            calidadMinima = calidadCandidata;
            ejeCandidato.Set(origen,destino);
            reiterar = true;
        }
        origen = nuevoCentro0Actual + (vcg::ClosestPoint(ejeActual,nuevoCentro0Actual)-nuevoCentro0Actual)*3/4;
        destino = nuevoCentro0Actual + (vcg::ClosestPoint(ejeActual,nuevoCentro0Actual)-nuevoCentro0Actual)*3/4;
        destino = destino - origen;
        destino = destino.Normalize();
        aux = -origen.Z()/destino.Z();
        origen = origen-destino*aux;
        ejeFinal.Set(origen,destino);
        calidadCandidata = calidadDelEje();
        if(calidadCandidata < calidadMinima){
            calidadMinima = calidadCandidata;
            ejeCandidato.Set(origen,destino);
            reiterar = true;
        }
        origen = nuevoCentro0Actual + (vcg::ClosestPoint(ejeActual,nuevoCentro0Actual)-nuevoCentro0Actual)*1/4;
        destino = nuevoCentro0Actual + (vcg::ClosestPoint(ejeActual,nuevoCentro0Actual)-nuevoCentro0Actual)*1/4;
        destino = destino - origen;
        destino = destino.Normalize();
        aux = -origen.Z()/destino.Z();
        origen = origen-destino*aux;
        ejeFinal.Set(origen,destino);
        calidadCandidata = calidadDelEje();
        if(calidadCandidata < calidadMinima){
            calidadMinima = calidadCandidata;
            ejeCandidato.Set(origen,destino);
            reiterar = true;
        }
        //elegir otros 9 ejes vecinos. ¿Cual es el criterio de ser vecino?
    }
    ejeFinal  = ejeCandidato;
    dibujarAristas = false;
    emit Imprimir("\nEl resultado al final es: " + QString::number(calidadMinima) + " ¡Bravo!\n");
}

float GLArea::reCalidadDelEje(int genetico){
    if(genetico == 0)
        GeneticoRefinaEje();
    else if(genetico == 1){
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
    float altura = 0;
    do{
        plano.Set(ejeFinal.Direction(),(altura * diagonal));
        altura = altura + 0.05;
    }while(InterseccionPlanoPieza(plano,0));
    altura = altura - 0.15;
    plano.Set(ejeFinal.Direction(),(altura * diagonal));
    InterseccionPlanoPieza(plano,0);
    altura = 0;
    do{
        plano.Set(ejeFinal.Direction(),(altura * diagonal));
        altura = altura - 0.05;
    }while(InterseccionPlanoPieza(plano,1));
    altura = altura + 0.15;
    plano.Set(ejeFinal.Direction(),(altura*diagonal));
    InterseccionPlanoPieza(plano,1);
}

float GLArea::calidadDelEje(){
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

    vcg::Point3f ejeX (1,0,0), aux;
    ejeRotacion = ejeFinal.Direction()^ejeX;
    vcg::Line3f lineaX;
    lineaX.Set(vcg::Point3f(0,0,0),ejeX);
    a = (ejeFinal.Direction() - vcg::ClosestPoint(lineaX,ejeFinal.Direction())).Norm();
    b = ejeFinal.Direction().X();
    theta = atan2(a,b);
    quaternion.FromAxis(theta,ejeRotacion);
    quaternionInverso.FromAxis(-theta,ejeRotacion);

    aux = quaternion.Rotate(ejeFinal.Direction());
    //dibujarEjeDePrueba = true;
    int contar = 0;
    for(int i=0; i<numPlanos; ++i){
        ei = aristacas[i].edges.begin();
        puntosOrdenadosDistancia.clear();
        while(ei!=aristacas[i].edges.end()){
            puntoNuevo = ei->P(0);
            ei++;
            puntosOrdenadosDistancia.insert((puntoNuevo - vcg::ClosestPoint(ejeFinal,puntoNuevo)).Norm(),puntoNuevo);
        }
        contar = puntosOrdenadosDistancia.size()/2. -1;
        it = puntosOrdenadosDistancia.begin();
        while(contar > 0){
            it = puntosOrdenadosDistancia.erase(it);
            contar--;
        }
        puntosDibujar[i] = puntosOrdenadosDistancia.values();
        emit Imprimir("\nComo va esto? " + QString::number(puntosOrdenadosDistancia.size()));
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



void GLArea::GrahamScan(QList<PuntoContornoLight> &puntos){
    PuntoContornoLight p0;
    //puntoGraham * primerPunto;
    QMap<PuntoContornoLight,PuntoContornoLight> puntosOrdenados;
    QMap<float,PuntoContornoLight> puntosAlturasOrdenados;
    int i, m, n, posMin = 0;
    float cat, alt, angulo;
    //Seleccionar el punto con menor altura y mas cerca del eje
    for(i=0; i<puntos.size(); ++i)
        if(puntos.at(i).altura < puntos.at(posMin).altura ||
           (puntos.at(i).altura == puntos.at(posMin).altura &&
            puntos.at(i).distancia < puntos.at(posMin).distancia))
                posMin = i;
    //se hace que ese punto este en el primer lugar
    p0.altura = puntos.at(0).altura;
    p0.distancia = puntos.at(0).distancia;
    puntos.replace(0,puntos.at(posMin));
    puntos.replace(posMin,p0);
    //Se ordenan el resto de puntos segun el angulo

    for(i=1; i<puntos.size(); ++i){
        cat = puntos[i].distancia - p0.distancia;
        alt = puntos[i].altura - p0.altura;
        puntos[i].angulo = atan2(alt,cat);
        puntos[i].dist2 = sqrt(cat*cat + alt*alt);
        puntosOrdenados.insert(puntos[i],puntos[i]);
    }
    puntos.clear();
    puntos.append(puntosOrdenados.values().at(puntosOrdenados.size()-1));
    puntos.append(puntosOrdenados.values());

    m = 2;
    n = puntosOrdenados.size();
    for(int i=3; i<=n; ++i){
        while(m > 1 && ccw(puntos.at(m-1),puntos.at(m),puntos.at(i))<=0)
            m--;

        m++;
        p0 = puntos.at(m);
        puntos[m] = puntos.at(i);
        puntos[i] = p0;
    }

    for(int i=n; i>m; --i){
        puntosAlturasOrdenados.insert(-puntos.at(i).altura,puntos.at(i));
        emit Imprimir("\nInsertando: " + QString::number(puntos.at(i).altura) + " " + QString::number(puntos.at(i).distancia));
        puntos.removeAt(i);
    }

    emit Imprimir("\nTamanio de esta cosa" + QString::number(m) + " " + QString::number(n) + " - " + QString::number(puntosAlturasOrdenados.size()));

    foreach(PuntoContornoLight puntito, puntosAlturasOrdenados){
        angulo = acos(ccw(puntos.at(m),puntito,puntos.at(0))) * 180./PI;
        emit Imprimir("\nAngulo: " + QString::number(angulo) + " Altura: " + QString::number(puntito.altura));
        if(angulo >= 90){
            puntos.append(puntito);
            m++;
        }
    }
}




