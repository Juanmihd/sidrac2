/****************************************************************************
 *                                                                           *
 ****************************************************************************/

#ifndef GLAREA_H_
#define GLAREA_H_

#include "estructurasDeDatos.h"
#include "puntocontorno.h"
#include "hebracalculoeje.h"


class GLArea:public QGLWidget
{
Q_OBJECT 
public:
  GLArea (QWidget * parent = 0);
  ~GLArea(){
      delete hebra;

  }
  /// we choosed a subset of the avaible drawing modes
  enum DrawMode{POINTS=0,WIRE,HIDDEN,FLATWIRE,FLAT,SMOOTH};
public slots:
  /// widget-based user interaction slots
  void calculoAcabado(QMultiMap<int,Voxel> voxelsFinal,
                      float valorMedioFinal,
                      vcg::Point3f nodoRaizFinal,
                      int maxInterseccionVoxelFinal,
                      vcg::Line3f ejeFinalFinal);
  void cambioModo(int modo);
  void loadMesh(QString filename, int tipoArchivo);
  void saveMesh(QString filename, int tipoArchivo);
  void cancelarSalir();
  void loadTetrahedron();
  void continuarRANSAC();
  void dibujaPlano();
  void cargarPlano();
  void inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje, bool refinarEje);
  void cambiaModoDibujado(){modoDibujado = !modoDibujado;updateGL();}
  void dibujaNormales();
  void cambiaValor(int valor);
  void alternarDibujoEje();
  void alternarDibujoVoxels();
  void clickado(int posx, int posy);
  void seleccionar();
  void calcularEje();
  void ResetContorno(){contorno.clear(); updateGL();}
  void ResetearPieza();
  void Generar(bool entera){CalcularContorno(entera);cambiaDibujo=!cambiaDibujo; updateGL();}
  void ReIniciarBarra(){emit IniciarBarra();}
  void ReAvanzarBarra(int valor){emit AvanzarBarra(valor);}
  void ReAcabarBarra(){emit AcabarBarra();}
  void ResetStatusBar(QString message){emit setStatusBar(message);}
  void ReImprimir(QString message){emit Imprimir(message);}
  float reCalidadDelEje(int genetico);
  void invertirEje(){
      ejeFinal.Set(ejeFinal.Direction()+ejeFinal.Origin(),ejeFinal.Direction()*(-1));
  }
signals:
  /// signal for setting the statusbar message
  void CrearConfiguracionEje(int *parametros);
  void setStatusBar(QString message);
  void setTextoInformacion(QString message);
  void Imprimir(QString message);
  void ImprimirHistorigrama(QString message);
  void IniciarBarra();
  void AvanzarBarra(int valor);
  void ajustarMaximoVoxeles(int valor);
  void AcabarBarra();
  void RatonEnGLArea();
  void activaBotonEje();
  void activaBotonVoxels();
protected:
  /// opengl initialization and drawing calls
  void initializeGL ();
  void resizeGL (int w, int h);
  void paintGL ();
  /// keyboard and mouse event callbacks
  void keyReleaseEvent(QKeyEvent * e);
  void keyPressEvent(QKeyEvent * e);
  void mousePressEvent(QMouseEvent*e);
  void mouseMoveEvent(QMouseEvent*e);
  void mouseReleaseEvent(QMouseEvent*e);
  void wheelEvent(QWheelEvent*e); 
private:
  hebraCalculoEje *hebra;
  /// Conjunto de normales
  QString nombreArchivo;
  QList<vcg::Line3f> normales;
  vcg::Point3f BB;
  vcg::Point3f BBsmall;
  vcg::Point3f posicionGlobal;
  QMap<puntoContorno,puntoContorno> contornoOrdenado;
  QList<PuntoContornoLight> puntosGrahamOrdenados;
  QList<PuntoContornoLight> puntosObtenidos;
  QList<puntoContorno> contorno;
  QMap<float, puntoContorno> contornoAlturas;
  QList<CMesh::FacePointer> carasSeleccionadas;
  int numPuntoContorno;
  int numCaras;
  puntoContorno puntoAlto;
  puntoContorno puntoBajo;
  vcg::Box3f caja;
  int maxInterseccionVoxel;
  int estadoDelCalculo;
  int valorColorVoxel;
  int **orientacionNormales;
  int tamanioOrientacionNormales;
  QMultiMap<int,Voxel> voxels;
  QList<Voxel> voxelsDentro;
  QList<Voxel> voxelsFuera;
  QList<vcg::Point3f> * puntosDibujar;
  int *** voxels2;
  bool **** voxelsNormal;
  bool botonIzquierdo;
  double nivelVoxels;
  float diagonal;
  int numNormales;
  int tamanioVoxel;
  int numVoxels;
  int antiguoTamanioVoxel;
  int cantidadDeIntersecciones;
  int filtroNormales;
  float anchoVoxels;
  float valorMedio;
  float calidadAnterior;
  vcg::Point3f nodoRaiz;
  /// Mallas
  CMesh mesh;
  CMesh mesh2;
  vcg::Line3f ejeFinal;
  vcg::Line3f ejeDePrueba;
  CMesh plano;
  CMesh figuraFinal;
  CMesh mallaEje;
  CEMesh * aristacas;
  bool* planosIntersecta;
  bool realizarGenetico;
  vcg::Point3f * centro;
  vcg::Point3f * nuevoCentro;
  vcg::Line3fN mallaEje2;
  /// the active mesh opengl wrapper
  vcg::GlTrimesh<CMesh> glWrap;
  vcg::GlTrimesh<CMesh> glWrapPlano;
  vcg::GlTrimesh<CMesh> glWrapEje;
  vcg::GlTrimesh<CMesh> glFigura;
  /// the active manipulator
  vcg::Trackball track;
  /// the current drawmode
  DrawMode drawmode;
  float errorEje;
  bool pintar;
  bool planoInterseccion;
  bool noMejora;
  bool voxelesDibujando;
  bool ejeDibujando;
  bool modoDibujado;
  bool normalesObtenidas;
  bool dibujarNormales;
  bool seleccionarActivado;
  bool cambiaDibujo;
  bool piezaCargada;
  bool dibujarEje;
  bool dibujarEjeDePrueba;
  bool dibujarVoxels;
  bool dibujarAristas;
  int numPlanos;
    // Variables para el calculo del eje
  float porcentajeDeseado;
  int limiteVoxelesInferior;
  float limiteVoxelesIntermedio;
  float limiteVoxelesEje;
  int limiteIteraciones;
  int amplitudVoxels;
  int amplitudMinima;
  bool ransacCalculado;
  int distanciaMinima;
  bool calcularV;
  bool calcularR;
  int parametrosCargados[9];

  /// mesh data structure initializer
  void initMesh(QString message);
  void ObtenerNormales();
  void CalcularContorno(bool entera);
  float Angulo(vcg::Point3f punto1, vcg::Point3f punto2);
  void GenerarPieza(int n);
  void GenerarPiezaEntera(int n);
  float Bezier3(float t, float P1, float P2, float P3);
  bool InterseccionPlanoPieza(vcg::Plane3f plano, int planoActual);
  float calidadDelEje();
  void GeneticoRefinaEje();
  void ObtenerPlanosCorte();
  float * ajusteCirculo(QList<vcg::Point3f> conjuntoPuntos);


void GrahamScan(QList<PuntoContornoLight> &puntos);
 };


#endif /*GLAREA_H_ */
