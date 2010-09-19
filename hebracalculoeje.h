/**
  * Autor: Juan Miguel Huertas Delgado
  * hebracalculoeje.cpp hebracalculoeje.h
  *
  * Esta es la clase hebra, que se encarga de realizar los cálculos de forma paralela al resto de la aplicación
  * Se comunica, con signals y slots
  * Hubo problemas con la devolución de parámetros, básicamente porque el resto de la aplicacion
  *
  */

#ifndef HEBRACALCULOEJE_H
#define HEBRACALCULOEJE_H

#include <QThread>

#include "estructurasDeDatos.h"

#ifndef PI
#define PI 3.141592
#endif

using namespace std;


/**
    @brief Esta clase es otra de las 3 fundamentales de la aplicación. Hace referencia a la hebra que calcula el eje de rotación, una de las
    funcionalidades basicas y más importantes de la aplicación
*/
class hebraCalculoEje : public QThread
{
    Q_OBJECT
public:
    /**
      @brief Este es el constructor de hebraCalculoEje
      @param parent : el padre de este widget
      */
    hebraCalculoEje(QObject *parent);
    /**
      @brief Inicializa el Eje
      */
    void inicializarEje();
    /**
      @brief Inicializa la semilla del aleatorio.
      */
    void inicializarSemilla(){
        int time = 2;//QTime::currentTime().second();
        qsrand(time);
        emit Imprimir("\nSemilla inicializada con : " + QString::number(time));
    }
    /**
      @brief Establece qué funcion deberá desempeñar la hebra cuando se inicialice
      @param i : el numero de la operacion
      */
    void setOperacion(int i, bool metodo){operacion = i;clusterMetodo = metodo;}
    /**
      @brief Fija los parámetros que se tendran en cuenta para la obtencion del eje de rotación
      @param parametros : es un vector de enteros con los parametros a tener en cuenta
                          limiteVoxelesInferior <= parametros[0]
                          limiteVoxelesIntermedio <= parametros[1]%
                          porcentajeDeseado <= parametros[2]%
                          limiteVoxelesEje <= parametros[3]%
                          limiteIteraciones <= parametros[4]
                          amplitudVoxels <= parametros[5]
                          amplitudMinima <= parametros[6]
                          calculoVoxels <= parametros[7]
                          calculoEje <= parametros[8]
      */
    void setParametros(int * parametros);
    /**
       @brief Inserta la malla del modelo, necesaria para el calculo
       @param glWrap2 : es la malla de la pieza
    */
    void insertarDatos(vcg::GlTrimesh<CMesh> glWrap2);
    /**
       @brief Permite insertar los voxels ya calculados a la hebra
       @param voxelsInicial : los voxels de partida
    */
    void insertarVoxels(QMultiMap<int,Voxel> voxelsInicial);

signals:
    /**
        @brief Esta señal se emite cuando la hebra termina, envia por los parametros todos los datos obtenidos por la hebra
        @param voxelsFinal : la nube de voxels calculada
        @param valorMedioFinal : el valor medio de los voxeles, para cuestiones de interfaz
        @param nodoRaizFinal : el tamaño del nodo que se ha usado
        @param maxInterseccionVoxelFinal : el maximo, para cuestiones de interfaz
        @param ejeFinalFinal : el eje de rotacion ya calculado
    */
    void finalizaHebra(QMultiMap<int,Voxel> voxelsFinal,
                       float valorMedioFinal,
                       vcg::Point3f nodoRaizFinal,
                       int maxInterseccionVoxelFinal,
                       vcg::Line3f ejeFinalFinal);
    /**
      @brief Inicia la barra de carga del sistema
    */
    void IniciarBarra();
    /**
      @brief Marca un valor a la barra de carga
      @param valor : debe ser un numero entre 0 y 100
    */
    void AvanzarBarra(int valor);
    /**
      @brief Finaliza la barra de carga del sistema
    */
    void AcabarBarra();
    /**
      @brief Imprime un mensaje en la barra de estado
      @param message : el mensaje a imprimir
    */
    void setStatusBar(QString message);
    /**
      @brief Imprime un mensaje en el log de la aplicacion
      @param message : el mensaje a imprimir
    */
    void Imprimir(QString message);

protected:
    /**
    @brief La funcion que, internamente, ejecuta la hebra cuando desde fuera se llama a start()
    */
    void run();

private:
    int operacion;
    QObject * padre;
    /**
      @brief Este metodo permite obtener el eje sin tener calculadas las normales. Un primer paso sera obtener las normales.
      */
    void ObtenerEjeSinNormales();
    /**
      @brief Antes de poder obtener el eje, este metodo inicializa los voxels
      */
    void InicializarVoxels();
    /**
      @brief Realiza el metodo de RANSAC
      */
    void RANSAC();
    /**
      @brief Metodo que selecciona los puntos punto1 y punto2
      @param punto1 : se pasa por referencia, al acabar obtendra el punto1
      @param punto2 : se pasa por referencia, al acabar obtendra el punto2
      */
    void SeleccionaPuntos(vcg::Point3f &punto1, vcg::Point3f & punto2);
    /**
      @brief Metodo que selecciona los puntos punto1 y punto2, este método tiene en cuenta la dirección del cluster de normales para obtener el eje de rotacion
      @param punto1 : se pasa por referencia, al acabar obtendra el punto1
      @param punto2 : se pasa por referencia, al acabar obtendra el punto2
      */
    void SeleccionaPuntosCluster(vcg::Point3f &punto1, vcg::Point3f & punto2);
    /**
      @brief Como parte final de Ransac se obtiene la distancia de los puntos a la recta, para seleccionar los que deben quedarse dentro y los que deben quedarse fuera
      @param recta : la recta que se analiza como "válida"
      @param distancia : la distancia que se exige como mínimo
      @param filtro : el filtro que se tiene que tener en cuenta para aceptar esta distancia como buena
      */
    void ComprobarDistancia(vcg::Line3f recta, float distancia, int filtro);
    /**
        @brief Obtiene, para cada normal, los voxels por los que pasa, incrementando el contador de dicho voxel
        @param normal : la normal a analizar
        @param caraActual : se le aporta tambien como información la cara actual, para futuras optimizaciones
      */
    void ObtenerVoxelsNormal(vcg::Line3f normal, int CaraActual);
    /**
      @brief Realiza una pequeña comparación, en función de un error para evitar puntos muy parecidos
      @param p1 : punto 1 a analizar
      @param p2 : punto 2 a analizar
      @param error : la distancia minima para considerarlos iguales
      @return devuelve true si se consideran iguales
    */
    bool MuyParecidos(vcg::Point3f p1, vcg::Point3f p2, float error){
        vcg::Point3f diferencia = p1-p2;
        return (diferencia.Norm() < error);
    }


    //Variables que hay que devolver
    QMultiMap<int,Voxel> voxels;
    float valorMedio;
    vcg::Point3f nodoRaiz;
    vcg::Line3f ejeFinal;

    // Parametros para el calculo del eje
    float porcentajeDeseado;
    int limiteVoxelesInferior;
    float limiteVoxelesIntermedio;
    float limiteVoxelesEje;
    int limiteIteraciones;
    int amplitudVoxels;
    int amplitudMinima;
    float distanciaMinima;
    bool contarClusterEje;
    bool calculoVoxels;
    bool clusterMetodo;
    bool calculoEje;
    float thetaGlobal;
    float fiGlobal;

    //Variables internas para el calculo del eje
    vcg::GlTrimesh<CMesh> glWrap;
    QList<vcg::Line3f> normales;
    QMap<PuntoOrdenable,PuntoOrdenable> orientacionNormales;
    int contadorNormales[360][360];
    int *** voxels2;
    int maxInterseccionVoxel;
    int anchoVoxels;
    vcg::Point3f BB;
    vcg::Point3f BBsmall;
    vcg::Point3f posicionGlobal;
    int antiguoTamanioVoxel;
    int nivelVoxels;
    QList<Voxel> voxelsDentro;
    QList<Voxel> voxelsFuera;
};

#endif // HEBRACALCULOEJE_H
