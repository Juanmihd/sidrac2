#ifndef HEBRACALCULOEJE_H
#define HEBRACALCULOEJE_H

#include <QThread>

#include "estructurasDeDatos.h"

#ifndef PI
#define PI 3.141592
#endif


class hebraCalculoEje : public QThread
{
    Q_OBJECT
public:
    hebraCalculoEje(QObject *parent);
    void inicializarEje();
    void setOperacion(int i){operacion = i;}
    void setParametros(int * parametros);
    void insertarDatos(vcg::GlTrimesh<CMesh> glWrap2);

signals:
    void finalizaHebra(QMultiMap<int,Voxel> voxelsFinal,
                       float valorMedioFinal,
                       vcg::Point3f nodoRaizFinal,
                       int maxInterseccionVoxelFinal,
                       vcg::Line3f ejeFinalFinal);
    void IniciarBarra();
    void AvanzarBarra(int valor);
    void AcabarBarra();
    void setStatusBar(QString message);
    void Imprimir(QString message);

protected:
    void run();

private:
    int operacion;
    QObject * padre;
    void ObtenerEjeSinNormales();
    void InicializarVoxels();
    void RANSAC();
    void SeleccionaPuntos(vcg::Point3f &punto1, vcg::Point3f & punto2);
    float ComprobarDistancia(vcg::Line3f recta, float distancia, int filtro);
    void ObtenerVoxelsNormal(vcg::Line3f normal, int CaraActual);
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
    bool calculoVoxels;
    bool calculoEje;

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
