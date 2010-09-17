/**
  * Autor: Juan Miguel Huertas Delgado
  * mainwindow.cpp mainwindow.h mainwindow.ui (y los generados automaticamente por este)
  *
  * Estos archivos contienen la ventana principal de la aplicación, con su sistema de menús.
  * Ademas contienen gran parte del controlador, que llama a una u otra función de glarea
  *
  *
  */


#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "ui_mainwindow.h"
#include "configuracion.h"
#include <QFile>
#include <QTextStream>
#include <QPushButton>


class MainWindow:public QMainWindow
{
Q_OBJECT 
public:
    /**
      @brief Constructor por defecto de la ventana principal
      @param parent : el padre de esta ventana
      */
  MainWindow(QWidget * parent = 0);
  /**
    Puntero a la ventana de configuracion del eje
    */
  Configuracion *configuracionEje;
signals:
  /**
    @brief Permite cargar una malla desde un archivo ya seleccionado
    @param newMesh : el nombre y ruta del archivo
    @param tipoArchivo : el tipo de extensión del archivo
    */
  void loadMesh(QString newMesh, int tipoArchivo);
  /**
    @brief Permite guardar una malla a un archivo ya seleccionado
    @param newMesh : el nombre y ruta del archivo
    @param tipoArchivo : el tipo de extensión del archivo
    */
  void saveMesh(QString newMesh, int tipoArchivo);
  /**
    @brief Si se deseaba salir, esta función se puede llamar para avisar de que se cancela la salida del sistema
    */
  void cancelarSalir();

protected:
  void resizeEvent(QResizeEvent * event);
private:
  int incrementoPosX;
  int estadoPieza;
  bool menusFijos;
  bool guardadoAsociado;
  int valorFiltro;
  int valorFiltro2;
  int valorFiltro3;
  int valorFiltro4;
  int * parametrosConfiguracion;
  int * parametrosPorDefecto;
  Ui::mainWindow ui;
  int modoVisualizacionActual;
  QActionGroup *grupoAlineamientoModo;
  bool parametrosEstimados;
  bool planoMostrado;
  bool pantallaCompleta;
  bool menusDinamicos;
  //Eventos de raton
  void mousePressEvent(QMouseEvent*e);
  void mouseMoveEvent(QMouseEvent*e);
  void mouseReleaseEvent(QMouseEvent*e);
  void wheelEvent(QWheelEvent*e);
  void SetMenuProyecto(bool valor);
  void SetMenuExportar(bool valor);
  void SetMenuPrograma(bool valor);
  void SetMenuReconstruccion(bool valor);
  void SetMenuEje(bool valor);
  void SetMenuContorno(bool valor);
  void SetMenu3D(bool valor);
  void SetMenuDibArq(bool valor);
  void SetMenuVisualizacion(bool valor);
  void SetMenuVReconstruccionEje(bool valor);
  void SetMenuVReconstruccionContorno(bool valor);
  void SetMenuModo(bool valor);
  void SetMenuAplicacion(bool valor);
  void SetElementos(bool valor);
  void ProyectoCargado(bool valor);
  void EjeCalculado(bool valor);
  void BloquearCalculos(bool valor);
  void TituloProyecto(QString texto);
  void OcultarBotones();
  QString nombrePieza;
  QString infoEje;
  QString infoContorno;
  QString infoFinal;
  QString nombreProyecto;
  QString rutaDeGuardado;
  QFile * salida;
  QFile * salida2;
  QFile * ficheroProyecto;
  QTextStream * outProyecto;
  QTextStream * out;
private slots:
    void on_BotonGuardarProyectoComo_clicked();
    void on_FijarVisualizacion_clicked();
    void on_FijarVisualizacion_2_clicked();
    void on_FijarConstruccion_2_clicked();
    void on_FijarConstruccion_clicked();
    void on_FijarPrograma_2_clicked();
    void on_FijarPrograma_clicked();
    void on_BotonMoverPieza_clicked();
    void on_BotonExportarDibujo_clicked();
    void on_BotonDeshacer_clicked();
    void on_BotonColor_clicked();
    void on_BotonContornoExterior_clicked();
    void on_BotonCalcularEje_clicked();
    void on_BotonCargarProyecto_clicked();
    void on_elementos5_pressed();
    void on_elementos3_pressed();
    void on_elementos4_pressed();
    void on_elementos2_pressed();
    void on_elementos1_pressed();
    void on_BotonElementos_clicked();
    void on_BotonNuevoProyecto_clicked();
    void on_BotonCerrarProyecto_clicked();
    void on_BotonGuardarProyecto_clicked();
    void on_BotonMenusDinamicos_clicked();
    void on_BotonMReconstruccionPulsado_clicked();
    void on_BotonMModoPulsado_clicked();
    void on_BotonMAplicacionPulsado_clicked();
    void on_BotonMReconstruccion_clicked();
    void on_BotonMModo_clicked();
    void on_BotonMAplicacion_clicked();
    void on_BotonAtras_Visualizacion_clicked();
    void on_BotonMEjePulsado_clicked();
    void on_BotonMContornoPulsado_clicked();
    void on_BotonM3DPulsado_clicked();
    void on_BotonMDibArqPulsado_clicked();
    void on_BotonAtras_Reconstruccion_clicked();
    void on_BotonMDibArq_clicked();
    void on_BotonM3D_clicked();
    void on_BotonMContorno_clicked();
    void on_BotonMEje_clicked();
    void on_BotonMProyectoPulsado_clicked();
    void on_BotonMExportarPulsado_clicked();
    void on_BotonMExportar_clicked();
    void on_BotonAtras_Programa_clicked();
    void on_BotonMProyecto_clicked();
    void on_BotonInvertir_clicked();
    void activaBotonEje();
    void activaBotonVoxels();
    void activaBotonContorno();
    void on_BotonDibujarEje_clicked();
    void on_BotonDibujarVoxels_clicked();
    void ajustarMaximoVoxeles(int valor);
    void on_sliderVoxeles_valueChanged(int value);
    void CierraMenus();
    void on_sliderVoxeles_sliderMoved(int position);
    void on_BotonGenerar_clicked();
    void on_BotonReset_clicked();
    void on_BotonEje2_clicked();
    void on_BotonContorno_clicked();
    void on_BotonNormales_clicked();
    void on_BotonDibujar_clicked();
    void on_BotonEje_clicked();
    void on_BotonTemporal_clicked();
    void on_BotonPantallaCompleta_clicked();
    bool on_BotonExportar_clicked();
    void on_BotonImportar_clicked();
    void on_BotonSalir_clicked();
    void Imprimir(QString texto);
    void ObtenerEje(QString info);
    void ObtenerFinal(QString info);
    void ObtenerContorno(QString info);
    void CrearConfiguracionEje(int * parametros);
    void ImprimirHistorigrama(QString texto);
    void IniciarBarra();
    void AvanzarBarra(int valor);
    void AcabarBarra();
    void obtenerPorDefecto();
    void ActivarReconstruccion();
    void inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje, bool refinarEje);
};

#endif /*MAINWINDOW_H_ */
