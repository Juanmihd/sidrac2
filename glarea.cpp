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

    qsrand(0);

    ResetearPieza();

    updateGL();

}

void GLArea::ResetearPieza(){
    puntoAlto.setAltura(-100000000);
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
                        glFigura.Draw<vcg::GLW::DMSmooth,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case POINTS:
                        glFigura.Draw<vcg::GLW::DMPoints,   vcg::GLW::CMNone,vcg::GLW::TMNone> ();
                        break;
                  case WIRE:
                        glFigura.Draw<vcg::GLW::DMWire,     vcg::GLW::CMNone,vcg::GLW::TMNone> ();
                        break;
                  case FLATWIRE:
                        glFigura.Draw<vcg::GLW::DMFlatWire, vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case HIDDEN:
                        glFigura.Draw<vcg::GLW::DMHidden,   vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  case FLAT:
                        glFigura.Draw<vcg::GLW::DMFlat,     vcg::GLW::CMPerFace,vcg::GLW::TMNone> ();
                        break;
                  default:
                        break;
                 }

            glPushMatrix();
            //Mover para que la pieza reconstruida quede centrada en el eje
            glRotatef(90,1,0,0);
            glTranslatef(0,-ejeFinal.Origin().Y(),0);
            /* //Dibujar el contorno cutre
            glColor3f(0,1,0);
            glBegin(GL_LINE_STRIP);
                foreach(puntoContorno puntoActual, contornoOrdenado){
                    glPushMatrix();
                    glVertex3f(puntoActual.getPosicion().X(),puntoActual.getPosicion().Y(),puntoActual.getPosicion().Z());
                    glPopMatrix();
                }
            glEnd();*/
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
                for(int j=0; j<3; ++j){
                    //if(planoIntersecta[j]){
                        glBegin(GL_POINTS);
                        for(int i=0; i<puntosDibujar[j].size(); ++i){
                                glVertex3f(puntosDibujar[j].value(i).X(),puntosDibujar[j].value(i).Y(),puntosDibujar[j].value(i).Z());
                        }
                        glEnd();
                    //}
                }
                glColor3f(0,1,0);
                glPointSize(10);
                glBegin(GL_POINTS);
                glVertex3f(centro[0].X(),centro[0].Y(),centro[0].Z());
                glVertex3f(centro[1].X(),centro[1].Y(),centro[1].Z());
                glEnd();
                glPointSize(1);
                glDisable(GL_POINT_SMOOTH);
                glBegin(GL_LINES);
                glVertex3f(centro[0].X(),centro[0].Y(),centro[0].Z());
                glVertex3f(centro[1].X(),centro[1].Y(),centro[1].Z());
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
  if(seleccionarActivado)
            clickado(e->x(), height() - e->y());
  else
        track.MouseDown (e->x (), height () - e->y (), QT2VCG (e->button (), e->modifiers ()));
  updateGL ();
}

void GLArea::mouseMoveEvent (QMouseEvent * e)
{
  if (e->buttons ()) {
      if(seleccionarActivado)
        clickado(e->x(), height() - e->y());
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
    clickado(e->x(), height() - e->y());
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

    CMesh::VertexIterator viM;
    vcg::Point3f min, max, actual;
    max.X() = max.Y() = max.Z() = 0;
    min.X() = min.Y() = min.Z() = 0;
    viM = mesh.vert.begin();
    while(viM != mesh.vert.end()){
        actual = viM->P();
        if(max.X() < actual.X())
            max.X() = actual.X();
        else if(min.X() > actual.X())
            min.X() = actual.X();
        if(max.Y() < actual.Y())
            max.Y() = actual.Y();
        else if(min.Y() > actual.Y())
            min.Y() = actual.Y();
        if(max.Z() < actual.Z())
            max.Z() = actual.Z();
        else if(min.Z() > actual.Z())
            min.Z() = actual.Z();
        viM ++;
    }
    float minDelTo = max.X()-min.X(), maxDelTo = max.X()-min.X();
    if(max.Y()-min.Y() < minDelTo)
        minDelTo = max.Y()-min.Y();
    if(max.Z()-min.Z() < minDelTo)
        minDelTo = max.Z()-min.Z();
    if(max.Y()-min.Y() > maxDelTo)
        maxDelTo = max.Y()-min.Y();
    if(max.Z()-min.Z() > maxDelTo)
        maxDelTo = max.Z()-min.Z();

    if(4*minDelTo <= maxDelTo)
        parametros[6] = 5;
    else if(2*minDelTo < maxDelTo)
        parametros[6] = 3;
    else
        parametros[6] = 0;
    emit CrearConfiguracionEje(parametros);
}

void GLArea::inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje){
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
    if(!voxelesDibujando){
        parametrosCargados[7] = true;
    }else
    parametrosCargados[7] = calcularVoxels;
    parametrosCargados[8] = calcularEje;
    if(calcularEje) ransacCalculado = true;

    if(!(calcularVoxels && calcularEje)){
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
            dibujarEje = true;
        }
    }else{

        if(hebra->isRunning())
            ;//emit AvisoError();
        else{
            hebra->setParametros(parametrosCargados);
            hebra->setOperacion(0);
            hebra->insertarDatos(glWrap);

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

    int contador = puntoAlto.getNumPunto();
    float altura = puntoAlto.getAltura();
    vcg::Point3f puntoEje = vcg::ClosestPoint(ejeFinal,puntoAlto.getPosicion());
    contornoOrdenado.clear();
        emit Imprimir(tr("\n"));
    foreach(puntoContorno punto,contorno){
        punto.setNumPunto(-contador);
        punto.setAltura(punto.getAltura() - altura);
        contador--;
        punto.setDistancia(vcg::Distance(ejeFinal,punto.getPosicion()));
        contornoOrdenado.insert(punto,punto);
    }

    if(entera)
        GenerarPiezaEntera(50);
    else
        GenerarPieza(50);
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
            (*fi).V(0)=ivp[numContornoVertices* i    + j];
            (*fi).V(1)=ivp[numContornoVertices* i    + j+1];
            (*fi).V(2)=ivp[numContornoVertices*(i+1) + j+1];
            ++fi;
            (*fi).V(0)=ivp[numContornoVertices* i    + j];
            (*fi).V(1)=ivp[numContornoVertices*(i+1) + j+1];
            (*fi).V(2)=ivp[numContornoVertices*(i+1) + j];
            ++fi;
        }
        (*fi).V(0)=ivp[numContornoVertices*n];
        (*fi).V(1)=ivp[numContornoVertices*(i)];
        (*fi).V(2)=ivp[numContornoVertices*(i+1)];
        ++fi;
        (*fi).V(0)=ivp[numContornoVertices*(i+2) - 1];
        (*fi).V(1)=ivp[numContornoVertices*(i+1) - 1];
        (*fi).V(2)=ivp[numContornoVertices*n + 1];
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

    (*fi).V(0)=ivp[numContornoVertices*n];
    (*fi).V(1)=ivp[numContornoVertices*(n-1)];
    (*fi).V(2)=ivp[0];
    ++fi;
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
        float altura;
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
            punto.setAltura(altura);
            punto.setNumPunto(numPuntoContorno);
            contorno.push_back(punto);
            if(altura > puntoAlto.getAltura()){
                puntoAlto.setAltura(altura);
                puntoAlto.setNumPunto(numPuntoContorno);
                puntoAlto.setPosicion(nuevoPunto);
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
        hebra->start();
    }else
        emit setStatusBar(tr("No se puede continuar calculando RANSAC si no se ha iniciado a calcular"));
}

void GLArea::calculoAcabado(QMultiMap<int,Voxel> voxelsFinal, float valorMedioFinal, vcg::Point3f nodoRaizFinal, int maxInterseccionVoxelFinal, vcg::Line3f ejeFinalFinal){
    voxels = voxelsFinal;
    dibujarEje = parametrosCargados[8];
    if(dibujarEje) emit activaBotonEje();
    dibujarVoxels = parametrosCargados[7];
    if(dibujarVoxels) emit activaBotonVoxels();
    voxelesDibujando = true;
    valorMedio = valorMedioFinal;
    nodoRaiz = nodoRaizFinal;
    maxInterseccionVoxel = maxInterseccionVoxelFinal;
    ejeFinal = ejeFinalFinal;
    emit ajustarMaximoVoxeles(maxInterseccionVoxel);
    valorColorVoxel = maxInterseccionVoxel - cantidadDeIntersecciones;
    valorColorVoxel = valorColorVoxel / 7;
    if(valorColorVoxel <= 0) valorColorVoxel = 1;

    emit Imprimir("\nLa calidad del eje: " + QString::number(calidadDelEje(0,2)));
    dibujarAristas = true;
}

float GLArea::calidadDelEje(float error, int numPlanos = 3){
    vcg::Point3f puntoNuevo, puntoViejo;
    vcg::Plane3f plano;
    aristacas = new CEMesh [numPlanos];
    int contador = 0;
    CEMesh::EdgeIterator ei;
    vcg::Quaternionf quaternion, quaternionInverso;
    vcg::Matrix33f matrizGrande;
    vcg::Point3f vectorDerecha,vectorIzquierda;
    float theta, fi;

 //   delete [] puntosDibujar;
 //   puntosDibujar = new QList<vcg::Point3f> [3];

    planoIntersecta = new bool [3];
    for(int i=0; i<3; ++i)
        planoIntersecta[i] = false;
    plano.Set(ejeFinal.Direction(),(-0.3 * diagonal));
    if(InterseccionPlanoPieza(plano,0)){
        contador++;
        planoIntersecta[0] = true;
    }
    plano.Set(ejeFinal.Direction(),(0 * diagonal));
    if(InterseccionPlanoPieza(plano,1)){
        contador++;
        planoIntersecta[1] = true;
    }
    plano.Set(ejeFinal.Direction(),(0.3 * diagonal));
    if(InterseccionPlanoPieza(plano,2)){
        contador++;
        planoIntersecta[2] = true;
    }

    if(contador >= 2){
        float x2, y2, x2y2,xy;
        for(int i=0; i<3; ++i){
            vectorDerecha[i] = 0;
            for(int j=0; j<3; ++j)
                matrizGrande[i][j] = 0;
        }

        fi = atan2(ejeFinal.Direction().X(),ejeFinal.Direction().Y());
        theta = sqrt(ejeFinal.Direction().X()*ejeFinal.Direction().X()+
                    ejeFinal.Direction().Y()*ejeFinal.Direction().Y()+
                    ejeFinal.Direction().Z()*ejeFinal.Direction().Z());
        theta = ejeFinal.Direction().Z()/theta;
        theta = theta<0?-acos(-theta):acos(theta);
        //emit Imprimir("Tiene que girarse "+QString::number(theta)+" "+QString::number(fi));
        quaternion.FromEulerAngles(-theta,-fi,0);
        quaternionInverso.FromEulerAngles(theta,fi,0);
        ejeDePrueba.Set(vcg::Point3f(0,0,0),quaternion.Rotate(ejeFinal.Direction()));
        for(int i=0; i<numPlanos; ++i){
            //if(planoIntersecta[i]){
                for(int k=0; k<3; ++k){
                    vectorDerecha[k] = 0;
                    for(int j=0; j<3; ++j)
                        matrizGrande[k][j] = 0;
                }
                ei = aristacas[i].edges.begin();
                while(ei!=aristacas[i].edges.end()){
                    puntoNuevo = ei->P(0);
                    puntosDibujar[i].append(puntoNuevo);
                    puntoNuevo = quaternion.Rotate(puntoNuevo);
                    ei++;
                    x2 = puntoNuevo.X()*puntoNuevo.X();
                    y2 = puntoNuevo.Y()*puntoNuevo.Y();
                    x2y2 = x2+y2;
                    xy = puntoNuevo.X()*puntoNuevo.Y();
                    matrizGrande[0][0]+=x2y2*x2y2;
                    matrizGrande[0][1]+=x2y2*puntoNuevo.X();
                    matrizGrande[0][2]+=x2y2*puntoNuevo.Y();
                    matrizGrande[1][1]+=x2;
                    matrizGrande[1][2]+=xy;
                    matrizGrande[2][2]+=y2;
                    vectorDerecha[0]+=x2y2;
                    vectorDerecha[1]+=puntoNuevo.X();
                    vectorDerecha[2]+=puntoNuevo.Y();
                }
                matrizGrande[2][1] = matrizGrande[1][2];
                matrizGrande[1][0] = matrizGrande[0][1];
                matrizGrande[2][0] = matrizGrande[0][2];
                matrizGrande = Inversa(matrizGrande);
                matrizGrande[2][1] = matrizGrande[1][2];
                matrizGrande[1][0] = matrizGrande[0][1];
                matrizGrande[2][0] = matrizGrande[0][2];
                vectorIzquierda = matrizGrande * vectorDerecha;
                centro[i].Z() = puntoNuevo.Z();
                centro[i].X() = (-vectorIzquierda[1])/(2*vectorIzquierda[0]);
                centro[i].Y() = (-vectorIzquierda[2])/(2*vectorIzquierda[0]);
                centro[i] = quaternionInverso.Rotate(centro[i]);
           // }
        }
    }

    return 0;
}

float GLArea::Determinante(float** mat, int tam){
    if(tam == 2){
        return mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
    }else if(tam == 3){
        return mat[0][0]*mat[1][1]*mat[2][2] +
               mat[1][0]*mat[2][1]*mat[0][2] +
               mat[2][0]*mat[0][1]*mat[1][2] -
               mat[2][1]*mat[1][1]*mat[0][2] -
               mat[0][0]*mat[2][1]*mat[1][2] -
               mat[1][0]*mat[0][1]*mat[2][2];
    }else
        return -1;
}

vcg::Matrix33f GLArea::Inversa(vcg::Matrix33f mat){
    vcg::Matrix33f mat2;
    float ** miniMatriz;
    float ** maxiMatriz;
    miniMatriz = new float*[2];
    miniMatriz[0] = new float[2];
    miniMatriz[1] = new float[2];
    maxiMatriz = new float*[3];
    for(int i=0; i<3; ++i){
        maxiMatriz[i] = new float [3];
        for(int j=0; j<3; ++j)
            maxiMatriz[i][j] = mat[i][j];
    }
    miniMatriz[0][0] = mat[1][1];
    miniMatriz[0][1] = mat[1][2];
    miniMatriz[1][0] = mat[2][1];
    miniMatriz[1][1] = mat[2][2];
    mat2[0][0] = Determinante(miniMatriz,2);
    miniMatriz[0][0] = mat[0][2];
    miniMatriz[0][1] = mat[0][1];
    miniMatriz[1][0] = mat[2][2];
    miniMatriz[1][1] = mat[2][1];
    mat2[0][1] = Determinante(miniMatriz,2);
    miniMatriz[0][0] = mat[0][1];
    miniMatriz[0][1] = mat[0][2];
    miniMatriz[1][0] = mat[1][1];
    miniMatriz[1][1] = mat[1][2];
    mat2[0][2] = Determinante(miniMatriz,2);

    miniMatriz[0][0] = mat[1][2];
    miniMatriz[0][1] = mat[1][0];
    miniMatriz[1][0] = mat[2][2];
    miniMatriz[1][1] = mat[2][0];
    mat2[1][0] = Determinante(miniMatriz,2);
    miniMatriz[0][0] = mat[0][0];
    miniMatriz[0][1] = mat[0][2];
    miniMatriz[1][0] = mat[2][0];
    miniMatriz[1][1] = mat[2][2];
    mat2[1][1] = Determinante(miniMatriz,2);
    miniMatriz[0][0] = mat[0][2];
    miniMatriz[0][1] = mat[0][0];
    miniMatriz[1][0] = mat[1][2];
    miniMatriz[1][1] = mat[1][0];
    mat2[1][2] = Determinante(miniMatriz,2);

    miniMatriz[0][0] = mat[1][0];
    miniMatriz[0][1] = mat[1][1];
    miniMatriz[1][0] = mat[2][0];
    miniMatriz[1][1] = mat[2][1];
    mat2[2][0] = Determinante(miniMatriz,2);
    miniMatriz[0][0] = mat[0][1];
    miniMatriz[0][1] = mat[0][0];
    miniMatriz[1][0] = mat[2][1];
    miniMatriz[1][1] = mat[2][0];
    mat2[2][1] = Determinante(miniMatriz,2);
    miniMatriz[0][0] = mat[0][0];
    miniMatriz[0][1] = mat[0][1];
    miniMatriz[1][0] = mat[1][0];
    miniMatriz[1][1] = mat[1][1];
    mat2[2][2] = Determinante(miniMatriz,2);

    mat2 = mat2 * Determinante(maxiMatriz,3);
    return mat2;
}



