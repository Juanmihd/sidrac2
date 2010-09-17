/**
  * Autor: Juan Miguel Huertas Delgado
  * glarea.cpp glarea.h
  *
  * Estos archivos contienen lo necesario para tener el modelo almacenado, y una parte muy pequeña del controlador
  * ademas esta clase es el visor openGL de la ventana de interfaz, que se ves desde otra ventana
  * a pesar de eso esta totalmente integrado con signals y slots de modo que es el otro el que, como controlador establece todas
  * las cosas que ocurren
  *
  */

#ifndef GLAREA_H_
#define GLAREA_H_

#include "estructurasDeDatos.h"
#include "puntocontorno.h"
#include "hebracalculoeje.h"

using namespace std;


/**
@brief Junto a mainwindow y hebraclaculoeje esta es una de las clases más importantes. Contiene al modelo y a parte del visualizador 3D

*/
class GLArea:public QGLWidget
{
Q_OBJECT 
public:
    /**
      @brief Este es el constructor de GLArea
      @param parent : el padre de este widget
      */
  GLArea (QWidget * parent = 0);

    /**
      @brief Destructor de la clase
      */
  ~GLArea(){
      delete hebra;
  }
  /// we choosed a subset of the avaible drawing modes
  enum DrawMode{POINTS=0,WIRE,HIDDEN,FLATWIRE,FLAT,SMOOTH};
public slots:
  /// widget-based user interaction slots
  /**
    @brief Esta funcion se enlaza al signal de la hebra, es la encargada de obtener los datos calculados
    @param voxelsFinal : los voxels calculados
    @param valorMedioFinal : el valor medio de los voxels
    @param nodoRaizFinal : el nodo raiz para calcular los voxels
    @param maxInterseccionVoxelFinal : con motivos de la interfaz
    @param ejeFinalFinal : el eje obtenido tras el proceso
    */
  void calculoAcabado(QMultiMap<int,Voxel> voxelsFinal,
                      float valorMedioFinal,
                      vcg::Point3f nodoRaizFinal,
                      int maxInterseccionVoxelFinal,
                      vcg::Line3f ejeFinalFinal);
    /**
      @brief Permite cambiar el modo de la visualizacion de la interfaz (YA NO SE USA, SE HACE AUTOMATICO EN MUCHOS CASOS)
      @param modo : el nuevo modo
      */
  void cambioModo(int modo);
  /**
    @brief Permite cargar una malla desde un archivo ya seleccionado
    @param filename : el nombre y ruta del archivo
    @param tipoArchivo : el tipo de extensión del archivo
    */
  void loadMesh(QString filename, int tipoArchivo);
  /**
    @brief Permite guardar una malla a un archivo ya seleccionado
    @param filename : el nombre y ruta del archivo
    @param tipoArchivo : el tipo de extensión del archivo
    */
  void saveMesh(QString filename, int tipoArchivo);
  /**
    @brief Este metodo se llama cuando se ha cancelado salir desde la interfaz
    */
  void cancelarSalir();
  /**
    @brief Este metodo se llama para refinar un poco mas el eje, segun el método RANSAC
    */
  void continuarRANSAC();
  /**
    @brief Este metodo inicializa el eje segun los parámetros que importa
    @param limInf : el valor minimo de intersecciones que se tendra en cuenta a la hora de calcular los "voxeles"
    @param limIntermedio : es el porcentaje de los voxeles calculados que se utilizaran a la hora de calcular el primer punto del eje
    @param porcentaje : es el procentaje de los puntos que deben de recoger el eje, en el calculo del RANSAC
    @param limiteIntersec : es el limite de intersecciones que se tienen en cuenta a la hora de calcular el primer punto del ransac, como maximo
    @param limiteIteracion : es el limite de iteracioens a la hora de realizar RANSAC
    @param amplitud : es la amplitud de espacio que abarca a la hora de calcular el RANSAC
    @param amplitudMin : es la amplitud minima en la cual no se tomaran intersecciones de los voxeles
    @param calcularVoxels : boolean que indica si se deben calcular los voxels o no
    @param calcularEje : boolean que indica si se debe calcular RANSAC o no
    @param refinarEje : boolean que indica que se va a refinar el eje (repetir RANSAC) o es la primera vez que se ejecuta
    */
  void inicializarEje(int limInf,
                      int limIntermedio,
                      int porcentaje,
                      int limiteIntersec,
                      int limiteIteracion,
                      int amplitud,
                      int amplitudMin,
                      bool calcularVoxels,
                      bool calcularEje,
                      bool refinarEje);
  /**
    @brief Cambia el modo de dibujado
    */
  void cambiaModoDibujado(){modoDibujado = !modoDibujado;updateGL();}
  /**
    @brief Dibuja (o deja de dibujar) las normales
    */
  void dibujaNormales();
  /**
    @brief Cambia el valor de las intersecciones
    @param valor : Nuevo valor de intersecciones
    */
  void cambiaValor(int valor);
  /**
    @brief Cambia entre ver y no ver el eje
    */
  void alternarDibujoEje();
  /**
    @brief Cambia entre ver y no ver los voxels
    */
  void alternarDibujoVoxels();
  /**
    @brief Se encarga de gestionar el click en el widget
    @param posx : Posicion x en la imagen
    @param posy : Posicion y en la imagen
    */
  void clickado(int posx, int posy);
  /**
    @brief Activa el paso de seleccion
    */
  void seleccionar();
  /**
    @brief Permite activar y desactivar la seleccion del contorno interior
    @param valor : Establece si se activa o no
    */
  void seleccionarInterior(bool valor);
  /**
    @brief Permite activar y desactivar la seleccion del contorno exterior
    @param valor : Establece si se activa o no
    */
  void seleccionarExterior(bool valor);
  /**
    @brief Comienza el proceso del calculo del eje
    */
  void calcularEje();
  /**
    @brief Estima los valores del eje
    */
  void calcularInicializarEje();
  /**
    @brief Resetea el contorno de la pieza
    */
  void ResetContorno();
  /**
    @brief Resetea todas las variables de la aplicacion
    */
  void ResetearPieza();
  /**
    @brief Se cierra el proyecto
    */
  void CerrarProyecto();
  /**
    @brief Finaliza el procesado de la pieza
    */
  void Generar();
  /**
    @brief Deshace un paso del contorno calculado
    */
  void deshacerContorno();
  /**
    @brief Inicia a cero la barra
    */
  void ReIniciarBarra(){emit IniciarBarra();}
  /**
    @brief Avanza la barra al valor
    @param valor : valor al que hay que avanzar la barra
    */
  void ReAvanzarBarra(int valor){emit AvanzarBarra(valor);}
  /**
    @brief Completa la barra de carga de la interfaz
    */
  void ReAcabarBarra(){emit AcabarBarra();}
  /**
    @brief Cambia la barra de estado
    @param message : texto a imprimir
    */
  void ResetStatusBar(QString message){emit setStatusBar(message);}
  /**
    @brief Imprime en el log
    @param message : texto a imprimir
    */
  void ReImprimir(QString message){emit Imprimir(message);}
  /**
    @brief Permite refinar la calidad del eje
    @param genetico : establece que metodo se debe usar
    @return Devuelve la calidad del eje
    */
  float reCalidadDelEje(int genetico);
  /**
    @brief Permite ver el dibujo arqueologico
    */
  void verDibujoArqueologico();
  /**
    @brief Permite ver el modelo en 3D
    */
  void verModeloReconstruido();
  /**
    @brief Invierte el eje de rotacion
    */
  void invertirEje(){
      invertido = !invertido;//ejeFinal.Set(ejeFinal.Direction()+ejeFinal.Origin(),ejeFinal.Direction()*(-1));
      alreves = -alreves;
      invertirMalla();
  }
  /**
    @brief Gira la pieza en el dibujo arqueologico
    @param valor : Determina si se debe de mover la pieza o no
    */
  void MoverPieza(bool valor);
  /**
    @brief Carga la informacion de un fichero que contenga informacion del eje
    @param valores : Informacion del fichero
    */
  void cargandoEje(float * valores);
  /**
    @brief Carga la informacion de un fichero que contenga informacion del eje
    @param valores : Informacion del fichero
    */
  void cargandoContorno(float * valores);
  /**
    @brief Carga la informacion de un fichero que contenga informacion del eje
    @param valores : Informacion del fichero
    */
  void cargandoFinal(float * valores);
  /**
    @brief Permite exportar una imagen en cualquier formato
    @param salida : Dispositivo de salida (un fichero ya abierto)
    @param formato : el formato en el que se desea guardar
    */
  void ExportarImagen(QIODevice * salida, const char * formato);
signals:
  /**
    @brief Se comienza a calcular al contorno, se avisa a la aplicación
    */
  void ContornoEmpiezaACalcularse();
  /**
    @brief Segun las caracteristicas de la pieza el sistema deduce que paramatros deben usarse
    @param parametros : devuelve por referencia los parametros estimados
    */
  void CrearConfiguracionEje(int *parametros);
  /**
    @brief Cambia el valor de la barra de estado
    @param message : texto a imprimir
    */
  void setStatusBar(QString message);
  /**
    @brief Establece la informacion de la pieza
    @param message : texto a imprimir
    */
  void setTextoInformacion(QString message);
  /**
    @brief Imprime en el log
    @param message : texto a imprimir
    */
  void Imprimir(QString message);
  /**
    @brief Para obtener el historigrama de la orientacion de normales se uso este metodo
    @param message : texto a imprimir
    */
  void ImprimirHistorigrama(QString message);
  /**
    @brief Se emite la información del eje, para que el controlador lo capte y guarde en un fichero
    */
  void InformacionEje(QString message);
  /**
    @brief Se emite la información del eje, para que el controlador lo capte y guarde en un fichero
    */
  void InformacionContorno(QString message);
  /**
    @brief Se emite la información del eje, para que el controlador lo capte y guarde en un fichero
    */
  void InformacionFinal(QString message);
  /**
    @brief Establece el valor maximo de voxels
    */
  void ajustarMaximoVoxeles(int valor);
  /**
    @brief Inicia a cero la barra
    */
  void IniciarBarra();
  /**
    @brief Avanza la barra al valor
    @param valor : valor al que hay que avanzar la barra
    */
  void AvanzarBarra(int valor);
  /**
    @brief Completa la barra de carga de la interfaz
    */
  void AcabarBarra();
  /**
    @brief Permite detectar si el raton está en GLArea y avisar a la interfaz
    */
  void RatonEnGLArea();
  /**
    @brief Esta señal se encarga de enviar informacion a la interfaz de que debe activar el boton del eje
    */
  void activaBotonEje();
  /**
    @brief Esta señal se encarga de enviar informacion a la interfaz de que debe activar el boton de voxels
    */
  void activaBotonVoxels();
  /**
    @brief Esta señal se encarga de enviar informacion a la interfaz de que debe activar el boton del contorno
    */
  void activaBotonContorno();
protected:
  /// opengl initialization and drawing calls
    /**
    @brief Inicializa todo lo necesario para la visualización 3D a través de openGL
    */
  void initializeGL ();
    /**
    @brief Realiza un resice de la aplicación en función de dos parametros (anchura y altura)
    @param w : anchura del espacio de dibujo
    @param h : altura del espacio de dibujo
    */
  void resizeGL (int w, int h);
    /**
    @brief Esta es la funcion encargada del dibujado. Se activa cuando se realiza un updateGL();
    */
  void paintGL ();
  /**
    @brief Se encarga de controlar cuando se suelta una tecla
    @param e : Evento de tecla
    */
  void keyReleaseEvent(QKeyEvent * e);
  /**
    @brief Se encarga de controlar cuando se pulsa una tecla
    @param e : Evento de tecla
    */
  void keyPressEvent(QKeyEvent * e);
  /**
    @brief Se encarga de controlar cuando se hace click en un boton del raton
    @param e : Evento de movimiento del raton
    */
  void mousePressEvent(QMouseEvent*e);
  /**
    @brief Se encarga de controlar el movimiento del raton
    @param e : Evento de movimiento del raton
    */
  void mouseMoveEvent(QMouseEvent*e);
  /**
    @brief Se encarga de controlar cuando se suelta un boton del raton
    @param e : Evento de movimiento del raton
    */
  void mouseReleaseEvent(QMouseEvent*e);
  /**
    @brief Se encarga de controlar el movimiento de la rueda del raton
    @param e : Evento de movimiento del raton
    */
  void wheelEvent(QWheelEvent*e); 
private:
  hebraCalculoEje *hebra;
  int tamPuntosm;
  int maxInterseccionVoxel;
  int estadoDelCalculo;
  int valorColorVoxel;
  int **orientacionNormales;
  int tamanioOrientacionNormales;
  int numPuntoContorno;
  int numCaras;
  int *** voxels2;
  int numNormales;
  int tamanioVoxel;
  int numVoxels;
  int antiguoTamanioVoxel;
  int cantidadDeIntersecciones;
  int filtroNormales;
  int limiteVoxelesInferior;
  int limiteIteraciones;
  int amplitudVoxels;
  int amplitudMinima;
  int distanciaMinima;
  int parametrosCargados[9];
  int numPlanos;
  int MAXCONTORNOS;
  int numContornos;
  int MAXCONTORNOSINT;
  int alreves;
  int numContornosInt;
  vcg::Point3f puntoOrigen;
  float anguloRotar;
  float zoom;
  float escala;
  float alturaCentrado;
  float alturaExtra;
  float alturaExtraOld;
  float anguloOld;
  float anguloPieza;
  float ratonYOld;
  float ratonXOld;
  float updown;
  float leftright;
  float diagonal;
  float anchoVoxels;
  float valorMedio;
  float calidadAnterior;
  float minGlobal;
  float maxGlobal;
  float distanciaMaxGlobal;
    // Variables para el calculo del eje
  float porcentajeDeseado;
  float limiteVoxelesIntermedio;
  float limiteVoxelesEje;
  float errorEje;
  float aspectRatio;
  GLfloat posicionLuz3D;
  GLfloat posicionLuzContorno;
  double nivelVoxels;
  bool moverPieza;
  bool invertido;
  bool mallaGirada;
  bool contornoInterior;
  bool contornoExterior;
  bool botonIzquierdo;
  bool* planosIntersecta;
  bool realizarGenetico;
  bool pintar;
  bool pintarContorno;
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
  /// Conjunto de normales
  QString nombreArchivo;
  QList<vcg::Point3f> * puntosDibujar;
  QList<vcg::Line3f> normales;
  QList<PuntoContornoLight> puntosObtenidos;
  QList<PuntoContornoLight> puntosExtras;
  QList<puntoContorno> * contornos;
  QList<puntoContorno> * contornosInt;
  QList<puntoContorno> contorno;
  QList<puntoContorno> contornoInt;
  QMap<float, puntoContorno> contornoAlturas;
  QMap<float, puntoContorno> contornoAlturasInt;
  QList<CMesh::FacePointer> carasSeleccionadas;
  QMap<puntoContorno,puntoContorno> contornoOrdenado;
  QMultiMap<int,Voxel> voxels;
  puntoContorno puntoAlto;
  puntoContorno puntoBajo;
  /// Mallas
  CMesh mesh;
  CMesh meshExt;
  CMesh mesh2;
  CMesh plano;
  CMesh figuraFinal;
  CMesh mallaEje;
  CEMesh * aristacas;
  vcg::Point3f * centro;
  vcg::Point3f * nuevoCentro;
  vcg::Point3f BB;
  vcg::Point3f BBsmall;
  vcg::Point3f posicionGlobal;
  vcg::Line3f ejeFinal;
  vcg::Line3f ejeDePrueba;
  vcg::Line3fN mallaEje2;
  vcg::Box3f caja;
  vcg::Point3f nodoRaiz;
  /// the active mesh opengl wrapper
  vcg::GlTrimesh<CMesh> glWrap;
  vcg::GlTrimesh<CMesh> glWrapRaro;
  vcg::GlTrimesh<CMesh> glWrapPlano;
  vcg::GlTrimesh<CMesh> glWrapEje;
  vcg::GlTrimesh<CMesh> glFigura;
  /// the active manipulator
  vcg::Trackball track;
  /// the current drawmode
  DrawMode drawmode;
  bool calcularV;
  bool calcularR;
  bool ransacCalculado;

  /// mesh data structure initializer
    /**
    @brief Inicializa una mesh auxiliar que permite obtener la seleccion de forma más eficiente
    */
  void initMeshSecundarias();
    /**
    @brief Inicializa la malla recien cargada en mesh
    @param message : El mensaje que se muestra en el statusbar
    */
  void initMesh(QString message);
    /**
    @brief Obtiene las normales del modelo
    */
  void ObtenerNormales();
    /**
    @brief Calcula el contorno en función de los puntos seleccionados
    */
  void CalcularContorno();
    /**
    @brief Genera la pieza reconstruida,
    @param n : Numero que determina la precision y calidad de la imagen
    */
  void GenerarPieza(int n);
    /**
    @brief Genera la pieza entera,
    @param n : Numero que determina la precision y calidad de la imagen
    */
  void GenerarPiezaEntera(int n);
    /**
    @brief Obtiene el valor de la curva de bezier, segun los valores P1, P2, P3 en funcion de t, a modo de curva parametrica
    @param t : Parametro que determina la curva
    @param P1 : Valor inicial
    @param P2 : Valor intermedio
    @param P3 : Valor final
    @return El valor para esos puntos y el parametro t
    */
  float Bezier3(float t, float P1, float P2, float P3);
    /**
    @brief Realiza la interseccion de un plano con el modelo. Guarda el resultado en la variable global aristacas[planoActual]
    @param plano : Esta variable representa a la estructura del plano, internamente es una dirección y un offset
    @param planoACtual : hacer referencia al numero de plano que se esta analizando
    @return Devuelve el valor booleano de si intersecta o no
    */
  bool InterseccionPlanoPieza(vcg::Plane3f plano, int planoActual);
    /**
    @brief Obtiene la calidad del eje y devuelve un float que indica esa calidad, si es -1 es que el eje es demasiado malo
    @return El valor de calidad
    */
  float calidadDelEje();
    /**
    @brief Obtiene los planos que se deben trazar para intersecta a la hora del calculo de calidad del eje
    */
  void ObtenerPlanosCorte();
    /**
    @brief Realiza un ajuste por minimos cuadrados de un conjunto de puntos
    @param conjuntoPuntos : El conjunto de puntos del cual se desea obtener el centro. Solo tiene en cuenta X e Y
    @return Un vector con tres valores, x e y del centro y el radio
    */
  float * ajusteCirculo(QList<vcg::Point3f> conjuntoPuntos);
    /**
    @brief Adapta el eje de forma que su origen y direccion no estén ni muy lejos de la pieza, ni dentro de su boundin box
    */
  void AdaptarEje();
    /**
    @brief Filtra el ruido de la malla de puntos que se le pasa como parametro
    @param puntos : La nube de puntos a filtrar y depurar
    */
  void FiltrarPuntos(QList<PuntoContornoLight> &puntos);
    /**
    @brief Envia a la interfaz la informacion del eje
    */
  void EmitirInformacionEje();
    /**
    @brief Envia a la interfaz la informacion del contorno
    */
  void EmitirInformacionContorno();
    /**
    @brief Envia a la interfaz la informacion del final
    */
  void EmitirInformacionFinal();
    /**
    @brief Gira la malla de seleccion para ajustarla al eje
    */
  void girarMalla();
    /**
    @brief Vuelve la malla de seleccion a su posicion inicial
    */
  void desgirarMalla();
    /**
    @brief Invierte la malla de seleccion (si ha habido que invertir el eje)
    */
  void invertirMalla();
    /**
    @brief Gira la malla de seleccion de modo exterior a modo interior
    */
  void cambioExtInt();
    /**
    @brief Esta función realiza un control de las variables contornos y contornosInt, que son vectores dinamicos
    Si hay algun problema con el tamaño, lo ajusta
    */
  void ComprobarContornos();
 };


#endif /*GLAREA_H_ */
