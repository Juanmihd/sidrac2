/****************************************************************************
 ****************************************************************************/

#include <QtGui>
#include "mainwindow.h"

MainWindow::MainWindow (QWidget * parent):QMainWindow (parent)
{
  incrementoPosX = 0;
  menusFijos = false;
  estadoPieza = -2;
  parametrosEstimados = false;
  valorFiltro = valorFiltro2 = 0;
  valorFiltro3 = 1;
  valorFiltro4 = 9;
  parametrosConfiguracion = new int[9];
  parametrosPorDefecto = new int[7];
  parametrosConfiguracion[0] = 2;
  parametrosConfiguracion[1] = 40;
  parametrosConfiguracion[2] = 80;
  parametrosConfiguracion[3] = 50;
  parametrosConfiguracion[4] = 1000;
  parametrosConfiguracion[5] = 10;
  parametrosConfiguracion[6] = 1;
  parametrosConfiguracion[7] = 1;
  parametrosConfiguracion[8] = 0;
    //inicializar semilla para probar cosas
  salida = new QFile("depurando.txt");
  salida->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
  ficheroProyecto = new QFile();
  ficheroTemp = new QFile ("temporal.sdc2");
  out = new QTextStream(salida);
  //int i=0;
  /*
  while(QFile::exists("HistorigramaNormales"+QString::number(i)+".txt"))i++;
  salida2 = new QFile("HistorigramaNormales"+QString::number(0)+".txt");
  salida2->open(QIODevice::ReadWrite | QIODevice::Text);
  out2 = new QTextStream(salida2);*/

    //maximiza la ventana
  ui.setupUi (this);
  planoMostrado = false;
  pantallaCompleta = menusDinamicos = false;
  modoVisualizacionActual = 0;

  //Elimiar las barras de los docks
  QWidget * borraBarra1 = new QWidget();
  QWidget * borraBarra2 = new QWidget();
  QWidget * borraBarra3 = new QWidget();
  ui.MenuInteractuar->setTitleBarWidget(borraBarra1);
  ui.MenuPrincipal->setTitleBarWidget(borraBarra2);
  ui.MenuVisualizacion->setTitleBarWidget(borraBarra3);

  //from toolFrame to glArea through mainwindow
  connect (this, SIGNAL (loadMesh(QString,int)),
           ui.glArea, SLOT(loadMesh(QString,int)));
  connect (this, SIGNAL (saveMesh(QString,int)),
           ui.glArea, SLOT(saveMesh(QString,int)));

  connect (ui.glArea, SIGNAL(ajustarMaximoVoxeles(int)),
           this, SLOT(ajustarMaximoVoxeles(int)));

  //para el cambio de modo dentro de herramientas

  connect (ui.glArea, SIGNAL(RatonEnGLArea()),
           this, SLOT(CierraMenus()));

  //from glArea to statusbar
  connect (ui.glArea, SIGNAL (setStatusBar(QString)),
           ui.statusbar, SLOT (showMessage(QString)));

  //from glArea to statusbar
  connect (ui.glArea, SIGNAL (setTextoInformacion(QString)),
           ui.textoInformacion, SLOT (setText(QString)));

  connect (ui.glArea, SIGNAL (Imprimir(QString)),
           this, SLOT (Imprimir(QString)));

  connect (ui.glArea, SIGNAL (InformacionEje(QString)),
           this, SLOT (ObtenerEje(QString)));

  connect (ui.glArea, SIGNAL (InformacionContorno(QString)),
           this, SLOT (ObtenerContorno(QString)));

  connect (ui.glArea, SIGNAL (ImprimirHistorigrama(QString)),
           this, SLOT (ImprimirHistorigrama(QString)));

  connect (ui.glArea, SIGNAL (IniciarBarra()),
           this, SLOT (IniciarBarra()));

  connect (ui.glArea, SIGNAL (AvanzarBarra(int)),
           this, SLOT (AvanzarBarra(int)));

  connect (ui.glArea, SIGNAL (AcabarBarra()),
           this, SLOT (AcabarBarra()));

  connect (ui.glArea, SIGNAL (CrearConfiguracionEje(int*)),
           this, SLOT (CrearConfiguracionEje(int*)));

  connect (ui.glArea, SIGNAL (activaBotonEje()),
           this, SLOT (activaBotonEje()));

  connect (ui.glArea, SIGNAL (activaBotonContorno()),
           this, SLOT (activaBotonContorno()));

  connect (ui.glArea, SIGNAL (activaBotonVoxels()),
           this, SLOT (activaBotonVoxels()));

  connect (ui.glArea, SIGNAL (ContornoEmpiezaACalcularse()),
           this, SLOT (ActivarReconstruccion()));

  connect(this, SIGNAL (cancelarSalir()),
          ui.glArea, SLOT(cancelarSalir()));
  ui.MenuPrincipal->setHidden(true);
  ui.MenuVisualizacion->setHidden(true);
  ui.MenuInteractuar->setHidden(true);
  ui.barraProgreso->setHidden(true);
  ui.glContorno->setHidden(true);

  ui.FijarConstruccion_2->setHidden(true);
  ui.FijarPrograma_2->setHidden(true);
  ui.FijarVisualizacion_2->setHidden(true);

  setWindowFlags(Qt::WindowMinMaxButtonsHint);

  SetMenuProyecto(false);
  SetMenuExportar(false);
  SetMenuEje(false);
  SetMenu3D(false);
  SetMenuContorno(false);
  SetMenuDibArq(false);
  SetElementos(false);

  SetMenuVReconstruccionEje(false);
  SetMenuVReconstruccionContorno(false);
  SetMenuModo(false);
  SetMenuAplicacion(false);
  OcultarBotones();
}

void MainWindow::OcultarBotones(){
    ui.FijarConstruccion->setAutoFillBackground(true);
    ui.FijarConstruccion->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.FijarConstruccion_2->setAutoFillBackground(true);
    ui.FijarConstruccion_2->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.FijarPrograma->setAutoFillBackground(true);
    ui.FijarPrograma->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.FijarPrograma_2->setAutoFillBackground(true);
    ui.FijarPrograma_2->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.FijarVisualizacion->setAutoFillBackground(true);
    ui.FijarVisualizacion->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.FijarVisualizacion_2->setAutoFillBackground(true);
    ui.FijarVisualizacion_2->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonAtras_Visualizacion->setAutoFillBackground(true);
    ui.BotonAtras_Visualizacion->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonAtras_Programa->setAutoFillBackground(true);
    ui.BotonAtras_Programa->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonAtras_Reconstruccion->setAutoFillBackground(true);
    ui.BotonAtras_Reconstruccion->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonCalcularEje->setAutoFillBackground(true);
    ui.BotonCalcularEje->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonCargarProyecto->setAutoFillBackground(true);
    ui.BotonCargarProyecto->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonCerrarProyecto->setAutoFillBackground(true);
    ui.BotonCerrarProyecto->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonColor->setAutoFillBackground(true);
    ui.BotonColor->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonContorno->setAutoFillBackground(true);
    ui.BotonContorno->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonContornoExterior->setAutoFillBackground(true);
    ui.BotonContornoExterior->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonDeshacer->setAutoFillBackground(true);
    ui.BotonDeshacer->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonDibujar->setAutoFillBackground(true);
    ui.BotonDibujar->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonDibujarEje->setAutoFillBackground(true);
    ui.BotonDibujarEje->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonDibujarVoxels->setAutoFillBackground(true);
    ui.BotonDibujarVoxels->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonEje->setAutoFillBackground(true);
    ui.BotonEje->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonEje2->setAutoFillBackground(true);
    ui.BotonEje2->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonElementos->setAutoFillBackground(true);
    ui.BotonElementos->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonExportar->setAutoFillBackground(true);
    ui.BotonExportar->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonExportarDibujo->setAutoFillBackground(true);
    ui.BotonExportarDibujo->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonGenerar->setAutoFillBackground(true);
    ui.BotonGenerar->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonGuardarProyecto->setAutoFillBackground(true);
    ui.BotonGuardarProyecto->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonGuardarProyectoComo->setAutoFillBackground(true);
    ui.BotonGuardarProyectoComo->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonM3D->setAutoFillBackground(true);
    ui.BotonM3D->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonM3DPulsado->setAutoFillBackground(true);
    ui.BotonM3DPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMAplicacion->setAutoFillBackground(true);
    ui.BotonMAplicacion->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMAplicacionPulsado->setAutoFillBackground(true);
    ui.BotonMAplicacionPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMetodo->setAutoFillBackground(true);
    ui.BotonMetodo->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMContorno->setAutoFillBackground(true);
    ui.BotonMContorno->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMContornoPulsado->setAutoFillBackground(true);
    ui.BotonMContornoPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMDibArq->setAutoFillBackground(true);
    ui.BotonMDibArqPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMedidas->setAutoFillBackground(true);
    ui.BotonMedidas->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMenusDinamicos->setAutoFillBackground(true);
    ui.BotonMenusDinamicos->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMEje->setAutoFillBackground(true);
    ui.BotonMEje->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMDibArq->setAutoFillBackground(true);
    ui.BotonMDibArq->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMDibArqPulsado->setAutoFillBackground(true);
    ui.BotonMDibArqPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMEjePulsado->setAutoFillBackground(true);
    ui.BotonMEjePulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMExportar->setAutoFillBackground(true);
    ui.BotonMExportar->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMExportarPulsado->setAutoFillBackground(true);
    ui.BotonMExportarPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMModo->setAutoFillBackground(true);
    ui.BotonMModo->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMModoPulsado->setAutoFillBackground(true);
    ui.BotonMModoPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMoverPieza->setAutoFillBackground(true);
    ui.BotonMoverPieza->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMProyecto->setAutoFillBackground(true);
    ui.BotonMProyecto->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMProyectoPulsado->setAutoFillBackground(true);
    ui.BotonMProyectoPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMReconstruccion->setAutoFillBackground(true);
    ui.BotonMReconstruccion->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonMReconstruccionPulsado->setAutoFillBackground(true);
    ui.BotonMReconstruccionPulsado->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonNormales->setAutoFillBackground(true);
    ui.BotonNormales->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonNuevoProyecto->setAutoFillBackground(true);
    ui.BotonNuevoProyecto->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonPantallaCompleta->setAutoFillBackground(true);
    ui.BotonPantallaCompleta->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonPerfil->setAutoFillBackground(true);
    ui.BotonPerfil->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonReset->setAutoFillBackground(true);
    ui.BotonReset->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonSalir->setAutoFillBackground(true);
    ui.BotonSalir->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonInvertir->setAutoFillBackground(true);
    ui.BotonInvertir->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonImportar->setAutoFillBackground(true);
    ui.BotonImportar->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonLuz->setAutoFillBackground(true);
    ui.BotonLuz->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonLuzDibujo->setAutoFillBackground(true);
    ui.BotonLuzDibujo->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
    ui.BotonTemporal->setAutoFillBackground(true);
    ui.BotonTemporal->setStyleSheet("background-color: rgb(0, 0, 0); color: rgb(255, 255, 255); border-color: rgb(0, 0, 0); border: black;");
}

void MainWindow::SetElementos(bool valor){
    valor = !valor;
    ui.elementos1->setHidden(valor);
    ui.elementos2->setHidden(valor);
    ui.elementos3->setHidden(valor);
    ui.elementos4->setHidden(valor);
    ui.elementos5->setHidden(valor);
}

void MainWindow::SetMenuProyecto(bool valor){
  valor = !valor;
  ui.BotonMProyectoPulsado->setChecked(true);
  ui.BotonMProyectoPulsado->setHidden(valor);
  ui.BotonAtras_Programa->setHidden(valor);
  ui.lineaPrograma->setHidden(valor);
  ui.lineaPrograma_2->setHidden(valor);
  ui.BotonCargarProyecto->setHidden(valor);
  ui.BotonGuardarProyecto->setHidden(valor);
  ui.BotonGuardarProyectoComo->setHidden(valor);
  ui.BotonCerrarProyecto->setHidden(valor);
  ui.BotonNuevoProyecto->setHidden(valor);
}

void MainWindow::SetMenuPrograma(bool valor){
  valor = !valor;
  ui.BotonAtras_Programa->setHidden(!valor);
  ui.lineaPrograma->setHidden(!valor);
  ui.lineaPrograma_2->setHidden(!valor);
  ui.BotonMProyecto->setHidden(valor);
  ui.BotonMExportar->setHidden(valor);
  ui.BotonSalir->setHidden(valor);
}

void MainWindow::SetMenuExportar(bool valor){
  valor = !valor;
  ui.BotonMExportarPulsado->setChecked(true);
  ui.BotonMExportarPulsado->setHidden(valor);
  ui.BotonExportar->setHidden(valor);
  ui.BotonExportarDibujo->setHidden(valor);
}

void MainWindow::SetMenuReconstruccion(bool valor){
  valor = !valor;
  ui.etiquetaProcesado->setHidden(valor);
  ui.etiquetaPostProcesado->setHidden(valor);
  ui.BotonMEje->setHidden(valor);
  ui.BotonM3D->setHidden(valor);
  ui.BotonMContorno->setHidden(valor);
  ui.BotonMDibArq->setHidden(valor);
}

void MainWindow::SetMenuEje(bool valor){
  valor = !valor;
  ui.BotonMEjePulsado->setChecked(true);
  ui.BotonMEjePulsado->setHidden(valor);
  ui.BotonCalcularEje->setHidden(valor);
  ui.BotonMetodo->setHidden(valor);
  ui.BotonEje->setHidden(valor);
  ui.BotonEje2->setHidden(valor);
  ui.BotonTemporal->setHidden(valor);
  ui.BotonInvertir->setHidden(valor);
  ui.BotonAtras_Reconstruccion->setHidden(valor);
  ui.lineaReconstruccion->setHidden(valor);
  ui.lineaReconstruccion_2->setHidden(valor);
}

void MainWindow::SetMenuContorno(bool valor){
  valor = !valor;
  ui.BotonMContornoPulsado->setChecked(true);
  ui.BotonMContornoPulsado->setHidden(valor);
  ui.BotonContorno->setHidden(valor);
  ui.BotonContornoExterior->setHidden(valor);
  ui.BotonReset->setHidden(valor);
  ui.BotonDeshacer->setHidden(valor);
  ui.BotonGenerar->setHidden(valor);
  ui.BotonAtras_Reconstruccion->setHidden(valor);
  ui.lineaReconstruccion->setHidden(valor);
  ui.lineaReconstruccion_2->setHidden(valor);
}

void MainWindow::SetMenu3D(bool valor){
  valor = !valor;
  ui.BotonM3DPulsado->setChecked(true);
  ui.BotonM3DPulsado->setHidden(valor);
  ui.BotonLuz->setHidden(valor);
  ui.BotonMesa->setHidden(valor);
  ui.BotonAtras_Reconstruccion->setHidden(valor);
  ui.lineaReconstruccion->setHidden(valor);
  ui.lineaReconstruccion_2->setHidden(valor);
}

void MainWindow::SetMenuDibArq(bool valor){
  valor = !valor;
  ui.BotonMDibArqPulsado->setChecked(true);
  ui.BotonMDibArqPulsado->setHidden(valor);
  ui.BotonLuzDibujo->setHidden(valor);
  ui.BotonMoverPieza->setHidden(valor);
  ui.BotonMedidas->setHidden(valor);
  ui.BotonAtras_Reconstruccion->setHidden(valor);
  ui.lineaReconstruccion->setHidden(valor);
  ui.lineaReconstruccion_2->setHidden(valor);
}

void MainWindow::SetMenuVisualizacion(bool valor){
  valor = !valor;
  ui.BotonMReconstruccion->setHidden(valor);
  ui.BotonMModo->setHidden(valor);
  ui.BotonMAplicacion->setHidden(valor);
}

void MainWindow::SetMenuVReconstruccionEje(bool valor){
  valor = !valor;
  ui.etiquetaEje->setHidden(valor);
  ui.BotonMReconstruccionPulsado->setChecked(true);
  ui.BotonMReconstruccionPulsado->setHidden(valor);
  ui.BotonDibujarVoxels->setHidden(valor);
  ui.etiquetaNumVoxeles->setHidden(valor);
  ui.sliderVoxeles->setHidden(valor);
  ui.tituloSliderVoxeles->setHidden(valor);
  ui.BotonNormales->setHidden(valor);
  ui.BotonDibujarEje->setHidden(valor);
  ui.BotonAtras_Visualizacion->setHidden(valor);
  ui.lineaVisualizacion->setHidden(valor);
  ui.lineaVisualizacion_2->setHidden(valor);
}

void MainWindow::SetMenuVReconstruccionContorno(bool valor){
  valor = !valor;
  ui.etiquetaContorno->setHidden(valor);
  ui.BotonMReconstruccionPulsado->setChecked(true);
  ui.BotonMReconstruccionPulsado->setHidden(valor);
  ui.BotonPerfil->setHidden(valor);
  ui.BotonDibujar->setHidden(valor);
  ui.BotonAtras_Visualizacion->setHidden(valor);
  ui.lineaVisualizacion->setHidden(valor);
  ui.lineaVisualizacion_2->setHidden(valor);
}

void MainWindow::SetMenuModo(bool valor){
  valor = !valor;
  ui.BotonMModoPulsado->setChecked(true);
  ui.BotonMModoPulsado->setHidden(valor);
  ui.BotonColor->setHidden(valor);
  ui.BotonElementos->setHidden(valor);
  ui.BotonAtras_Visualizacion->setHidden(valor);

  ui.lineaVisualizacion->setHidden(valor);
  ui.lineaVisualizacion_2->setHidden(valor);
}

void MainWindow::SetMenuAplicacion(bool valor){
  valor = !valor;
  ui.BotonMAplicacionPulsado->setChecked(true);
  ui.BotonMAplicacionPulsado->setHidden(valor);
  ui.BotonPantallaCompleta->setHidden(valor);
  ui.BotonMenusDinamicos->setHidden(valor);
  ui.BotonAtras_Visualizacion->setHidden(valor);
  ui.lineaVisualizacion->setHidden(valor);
  ui.lineaVisualizacion_2->setHidden(valor);
}

/*
void MainWindow::on_actionAcerca_de_activated()
{
     QMessageBox::about(this, tr("Sidrac 2"),
                        tr("Proyecto fin de carrera\nRealizado por Juan Miguel Huertas Delgado.\nVersion 2.0.1"));
}

void MainWindow::on_actionAcerca_de_Qt_activated()
{
     QMessageBox::aboutQt(this);
}
*/

void MainWindow::resetearTodo(){
    estadoPieza = -2;
    ui.BotonExportar->setEnabled(false);
    ui.BotonM3D->setEnabled(false);
    ui.BotonMDibArq->setEnabled(false);
    ui.BotonMReconstruccion->setEnabled(false);
    ui.BotonMEje->setEnabled(false);
    ui.BotonMContorno->setEnabled(false);
}

void MainWindow::on_BotonSalir_clicked()
{
     int reply;
     reply = QMessageBox::question(this, tr("Salir"),
                                     tr("¿Realmente desea salir de la aplicación sin guardar?\n (Perderá los cambios no guardados)"),
                                     tr("Salir sin guardar"), tr("Guardar y salir"), tr("Volver a la aplicacion"));
     if (reply == 0){
         salida->close();
         ficheroTemp->close();
         ficheroTemp->remove();
         /*if(salida2->isOpen())
            salida2->close();*/
         if(ficheroProyecto->isOpen()){
            ficheroProyecto->close();
            //QFile::remove(ficheroProyecto->fileName());
         }
         exit(0);
     }else if (reply == 1){
         salida->close();
         ficheroTemp->close();
         ficheroTemp->remove();
        /* if(salida2->isOpen())
            salida2->close();*/
         on_BotonGuardarProyecto_clicked();
         if(ficheroProyecto->isOpen())
            ficheroProyecto->close();
         exit(0);
     }else
         emit cancelarSalir();
}

void MainWindow::on_BotonImportar_clicked()
{
    ui.glArea->setCursor(QCursor(Qt::SizeAllCursor));

    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Importar modelo"), QDir::currentPath(),
                    tr("Modelo ply (*.ply)"));
    if(!fileName.isEmpty()){
        if(QFile::exists(fileName)){
            resetearTodo();
            ProyectoCargado(true);
            PiezaImportada();
            emit loadMesh(fileName,0);
            ui.BotonMEje->setEnabled(true);
            ui.BotonDibujar->setEnabled(true);
            ui.BotonDibujarEje->setEnabled(false);
            ui.BotonDibujarVoxels->setEnabled(false);
            ui.BotonNormales->setEnabled(true);
            parametrosEstimados = false;
            nombrePieza.clear();
            nombrePieza.append(fileName);
            estadoPieza = 0;
        }else{
            ui.statusbar->showMessage(tr("No se encuentra el fichero ") + fileName);
        }
    }
}

bool MainWindow::on_BotonExportar_clicked()
{
    on_BotonMExportarPulsado_clicked();
    QString fileName = QFileDialog::getSaveFileName(this,
                    tr("Exportar modelo"), QDir::currentPath(),
                    tr("Modelo ply (*.ply)"));
  if(!fileName.isEmpty()){
    if(fileName.endsWith(".ply"))
        emit saveMesh(fileName,0);
    return true;
  }
  else return false;  
}

void MainWindow::mousePressEvent(QMouseEvent*e)
{}
void MainWindow::mouseReleaseEvent(QMouseEvent*e)
{}
void MainWindow::wheelEvent(QWheelEvent*e)
{}
void MainWindow::mouseMoveEvent (QMouseEvent * e)
{
      int posy = e->x(), posx = e->y(), posx2 = height() - posx, posy2 = width() - posy;
      bool activado = false;
      if(ui.MenuPrincipal->isHidden()){
          if(posy <30){
            emit ui.etiquetaPrincipal->setHidden(true);
            emit ui.MenuPrincipal->setHidden(false);
            activado = true;
          }

      }else{
          if(!(posy < 110) & !ui.FijarPrograma->isChecked()){
            emit ui.MenuPrincipal->setHidden(true);
            emit ui.etiquetaPrincipal->setHidden(false);
            activado = true;
              }
          }
      if(!activado){
          if(ui.MenuVisualizacion->isHidden()){
              if(posx2 < 50){
                  ui.etiquetaVisualizacion->setHidden(true);
                  ui.MenuVisualizacion->setHidden(false);
                  activado = true;
              }
          }else{
              if(!(posx2 < 110) && !ui.FijarVisualizacion->isChecked()){
                  ui.MenuVisualizacion->setHidden(true);
                  ui.etiquetaVisualizacion->setHidden(false);
                  activado = true;
              }
          }
      }
      if(!activado){
          if(ui.MenuInteractuar->isHidden()){
              if(posy2 <= 30){
                  emit ui.etiquetaInteractuar->setHidden(true);
                  emit ui.MenuInteractuar->setHidden(false);
                  activado = true;
              }
          }else if(!(posy2 <110) && !ui.FijarConstruccion->isChecked()){
                  emit ui.MenuInteractuar->setHidden(true);
                  emit ui.etiquetaInteractuar->setHidden(false);
                  activado = true;
              }
      }
  }

void MainWindow::CierraMenus(){
    if(!ui.elementos1->isHidden()){
        SetElementos(false);
        SetMenuVisualizacion(true);
    }
    if(!ui.FijarConstruccion->isChecked()){
        emit ui.MenuInteractuar->setHidden(true);
        emit ui.etiquetaInteractuar->setHidden(false);
    }
    if(!ui.FijarPrograma->isChecked()){
        emit ui.MenuPrincipal->setHidden(true);
        emit ui.etiquetaPrincipal->setHidden(false);
    }
    if(!ui.FijarVisualizacion->isChecked()){
        emit ui.MenuVisualizacion->setHidden(true);
        emit ui.etiquetaVisualizacion->setHidden(false);
    }
}

void MainWindow::on_BotonPantallaCompleta_clicked()
{
    if(pantallaCompleta)
        showMaximized();
    else
        showFullScreen();
    pantallaCompleta=!pantallaCompleta;
    on_BotonMAplicacionPulsado_clicked();
}

void MainWindow::resizeEvent(QResizeEvent * event){
    float variacionAnchura = this->width() - event->oldSize().width();
    if(ui.dockWidgetContents_3->x()+variacionAnchura/2. > 30)
        ui.dockWidgetContents_3->move(ui.dockWidgetContents_3->x()+variacionAnchura/2.,0);
    else
        ui.dockWidgetContents_3->move(30,0);
}

void MainWindow::on_BotonTemporal_clicked()
{
    on_BotonMEjePulsado_clicked();
    emit ui.glArea->continuarRANSAC();
    BloquearCalculos(true);
}

void MainWindow::on_BotonEje_clicked()
{
    on_BotonMEjePulsado_clicked();
    if(parametrosEstimados)
        CrearConfiguracionEje(0);
    else{
        emit ui.glArea->calcularEje();
        parametrosEstimados = true;
    }
    ui.BotonEje2->setEnabled(true);
    ui.BotonTemporal->setEnabled(true);
}

void MainWindow::obtenerPorDefecto(){
    emit configuracionEje->porDefecto(parametrosPorDefecto);
}

void MainWindow::CrearConfiguracionEje(int * parametros){
    if(parametros!=0){
        for(int i=0; i<7; ++i){
            parametrosConfiguracion[i] = parametros[i];
            parametrosPorDefecto[i] = parametros[i];
        }
    }
    configuracionEje = new Configuracion (this,parametrosConfiguracion);
    configuracionEje->exec();
}

void MainWindow::on_BotonEje2_clicked()
{
    emit ui.glArea->reCalidadDelEje(0);
}

void MainWindow::on_BotonDibujar_clicked()
{
    emit ui.glArea->cambiaModoDibujado();
}

void MainWindow::Imprimir(QString texto){
    (*out) << texto;
    out->flush();
}

void MainWindow::ImprimirHistorigrama(QString texto){
   // (*out2) << texto;
}

void MainWindow::on_BotonNormales_clicked()
{
    emit ui.glArea->dibujaNormales();
}

void MainWindow::IniciarBarra(){
    ui.barraProgreso->setHidden(false);
}

void MainWindow::AvanzarBarra(int valor){
    ui.barraProgreso->setValue(valor);
}

void MainWindow::AcabarBarra(){
    ui.barraProgreso->setHidden(true);
}

void MainWindow::on_BotonReset_clicked()
{
    emit ui.glArea->ResetContorno();
}

void MainWindow::on_BotonGenerar_clicked()
{
    if(ui.BotonContorno->isChecked()){
        ui.BotonContorno->setChecked(false);
        ui.glArea->setCursor(QCursor(Qt::SizeAllCursor));
        emit ui.glArea->seleccionar();
    }
    emit ui.glArea->Generar();
    on_BotonMContornoPulsado_clicked();
}

void MainWindow::inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje, bool refinarEje){
    parametrosConfiguracion[0] = limInf;
    parametrosConfiguracion[1] = limIntermedio;
    parametrosConfiguracion[2] = porcentaje;
    parametrosConfiguracion[3] = limiteIntersec;
    parametrosConfiguracion[4] = limiteIteracion;
    parametrosConfiguracion[5] = amplitud;
    parametrosConfiguracion[6] = amplitudMin;
    parametrosConfiguracion[7] = calcularVoxels;
    parametrosConfiguracion[8] = calcularEje;

    ui.BotonTemporal->setEnabled(true);
    ui.BotonReset->setEnabled(true);
    ui.BotonGenerar->setEnabled(true);
    ui.BotonDibujarVoxels->setEnabled(true);
    ui.sliderVoxeles->setEnabled(true);
    ui.tituloSliderVoxeles->setEnabled(true);
    BloquearCalculos(true);
    emit ui.glArea->inicializarEje(limInf,limIntermedio,porcentaje,limiteIntersec,limiteIteracion,amplitud,amplitudMin,calcularVoxels,calcularEje,refinarEje,ui.BotonMetodo->isChecked());
}

void MainWindow::on_sliderVoxeles_sliderMoved(int position)
{
    emit ui.glArea->cambiaValor(position);
    ui.etiquetaNumVoxeles->setText(QString::number(position));
}

void MainWindow::ajustarMaximoVoxeles(int valor){
    ui.etiquetaNumVoxeles->setText(QString::number(parametrosConfiguracion[0]));
    ui.sliderVoxeles->setRange(parametrosConfiguracion[0],valor);
    ui.sliderVoxeles->setValue(parametrosConfiguracion[0]);
}

void MainWindow::on_sliderVoxeles_valueChanged(int value)
{
    on_sliderVoxeles_sliderMoved(value);
}

void MainWindow::on_BotonDibujarVoxels_clicked()
{
    ui.sliderVoxeles->setEnabled(ui.BotonDibujarVoxels->isChecked());
    ui.etiquetaNumVoxeles->setEnabled(ui.BotonDibujarVoxels->isChecked());
    ui.tituloSliderVoxeles->setEnabled(ui.BotonDibujarVoxels->isChecked());
    emit ui.glArea->alternarDibujoVoxels();
}

void MainWindow::on_BotonDibujarEje_clicked()
{
    emit ui.glArea->alternarDibujoEje();
}

void MainWindow::on_BotonInvertir_clicked()
{
    emit ui.glArea->invertirEje();
}

void MainWindow::on_BotonMProyecto_clicked()
{
    SetMenuPrograma(false);
    SetMenuProyecto(true);
}

void MainWindow::on_BotonAtras_Programa_clicked()
{
    SetMenuProyecto(false);
    SetMenuExportar(false);
    SetMenuPrograma(true);
}

void MainWindow::on_BotonMExportar_clicked()
{
    SetMenuPrograma(false);
    SetMenuExportar(true);
}

void MainWindow::on_BotonMExportarPulsado_clicked()
{
    SetMenuExportar(false);
    SetMenuPrograma(true);
}

void MainWindow::on_BotonMProyectoPulsado_clicked()
{
    SetMenuProyecto(false);
    SetMenuPrograma(true);
}

void MainWindow::on_BotonMEje_clicked()
{
    SetMenuReconstruccion(false);
    SetMenuEje(true);
}

void MainWindow::on_BotonMContorno_clicked()
{
    SetMenuReconstruccion(false);
    SetMenuContorno(true);
}

void MainWindow::on_BotonM3D_clicked()
{
    SetMenuReconstruccion(false);
    SetMenu3D(true);
    emit ui.glArea->verModeloReconstruido();
}

void MainWindow::on_BotonMDibArq_clicked()
{
    SetMenuReconstruccion(false);
    SetMenuDibArq(true);
    emit ui.glArea->verDibujoArqueologico();
}

void MainWindow::on_BotonAtras_Reconstruccion_clicked()
{
    if(ui.BotonMoverPieza->isChecked()){
        ui.BotonMoverPieza->setChecked(false);
        on_BotonMoverPieza_clicked();
    }
    SetMenuDibArq(false);
    SetMenu3D(false);
    SetMenuEje(false);
    SetMenuContorno(false);
    SetMenuReconstruccion(true);
}

void MainWindow::on_BotonMDibArqPulsado_clicked()
{
    if(ui.BotonMoverPieza->isChecked()){
        ui.BotonMoverPieza->setChecked(false);
        on_BotonMoverPieza_clicked();
    }
    SetMenuDibArq(false);
    SetMenuReconstruccion(true);
}

void MainWindow::on_BotonM3DPulsado_clicked()
{
    SetMenu3D(false);
    SetMenuReconstruccion(true);
}

void MainWindow::on_BotonMContornoPulsado_clicked()
{
    SetMenuContorno(false);
    SetMenuReconstruccion(true);
}

void MainWindow::on_BotonMEjePulsado_clicked()
{
    SetMenuEje(false);
    SetMenuReconstruccion(true);
}

void MainWindow::on_BotonAtras_Visualizacion_clicked()
{
    SetMenuVReconstruccionEje(false);
    SetMenuVReconstruccionContorno(false);
    SetMenuModo(false);
    SetMenuAplicacion(false);
    SetMenuVisualizacion(true);
}

void MainWindow::on_BotonMAplicacion_clicked()
{
    SetMenuVisualizacion(false);
    SetMenuAplicacion(true);
}

void MainWindow::on_BotonMModo_clicked()
{
    SetMenuVisualizacion(false);
    SetMenuModo(true);
}

void MainWindow::on_BotonMReconstruccion_clicked()
{
    SetMenuVisualizacion(false);
    if(estadoPieza == 1)
        SetMenuVReconstruccionEje(true);
    else if(estadoPieza == 2)
        SetMenuVReconstruccionContorno(true);
}

void MainWindow::on_BotonMAplicacionPulsado_clicked()
{
    SetMenuAplicacion(false);
    SetMenuVisualizacion(true);
}

void MainWindow::on_BotonMModoPulsado_clicked()
{
    SetMenuModo(false);
    SetMenuVisualizacion(true);
}

void MainWindow::on_BotonMReconstruccionPulsado_clicked()
{
    SetMenuVReconstruccionEje(false);
    SetMenuVReconstruccionContorno(false);
    SetMenuVisualizacion(true);
}

void MainWindow::on_BotonMenusDinamicos_clicked()
{
    menusDinamicos = !menusDinamicos;
    menusFijos = ui.BotonMenusDinamicos->isChecked();
    ui.etiquetaInteractuar->setHidden(menusFijos);
    ui.etiquetaPrincipal->setHidden(menusFijos);
    ui.MenuInteractuar->setHidden(!menusFijos);
    ui.MenuPrincipal->setHidden(!menusFijos);
    ui.MenuVisualizacion->setHidden(!menusFijos);
    ui.FijarConstruccion->setChecked(menusFijos);
    ui.FijarVisualizacion->setChecked(menusFijos);
    ui.FijarPrograma->setChecked(menusFijos);
    ui.FijarConstruccion->setHidden(menusFijos);
    ui.FijarVisualizacion->setHidden(menusFijos);
    ui.FijarPrograma->setHidden(menusFijos);
    ui.FijarConstruccion_2->setHidden(!menusFijos);
    ui.FijarVisualizacion_2->setHidden(!menusFijos);
    ui.FijarPrograma_2->setHidden(!menusFijos);
    on_BotonMAplicacionPulsado_clicked();
}

void MainWindow::TituloProyecto(QString texto){
        nombreProyecto.clear();
        nombreProyecto.append(texto);
        ui.tituloProyecto->setText(nombreProyecto);
        texto.prepend("SIDRAC 2.0 - Proyecto de nombre ");
        setWindowTitle(texto);
}

void MainWindow::ProyectoCargado(bool valor){
    ui.BotonGuardarProyecto->setEnabled(valor);
    ui.BotonGuardarProyectoComo->setEnabled(valor);
    ui.BotonMModo->setEnabled(valor);
    ui.BotonCerrarProyecto->setEnabled(valor);
    ui.BotonImportar->setEnabled(valor);
    estadoPieza = -1;
}

void MainWindow::PiezaImportada(){
    ui.BotonMReconstruccion->setEnabled(true);
    ui.BotonMEje->setEnabled(true);
    estadoPieza = 0;
}

void MainWindow::EjeCalculado(bool valor){
    ProyectoCargado(true);
    ui.BotonMReconstruccion->setEnabled(valor);
    ui.BotonMContorno->setEnabled(valor);
    ui.BotonContorno->setEnabled(valor);
    ui.BotonDibujarEje->setEnabled(valor);
    estadoPieza = 1;
    GuardadoTemporal();
}

void MainWindow::activaBotonEje(){
    ui.BotonDibujarEje->setEnabled(true);
}

void MainWindow::activaBotonContorno(){
    EjeCalculado(true);
    ui.BotonDibujarVoxels->setEnabled(true);
    ui.sliderVoxeles->setEnabled(true);
    ui.tituloSliderVoxeles->setEnabled(true);
    BloquearCalculos(false);
}

void MainWindow::activaBotonVoxels(){
    ui.BotonDibujarVoxels->setEnabled(true);
    ui.sliderVoxeles->setEnabled(true);
}

void MainWindow::ObtenerEje(QString info){
    infoEje.clear();
    infoEje.append(info);
}

void MainWindow::ObtenerContorno(QString info){
    infoContorno.clear();
    infoContorno.append(info);
    GuardadoTemporal();
}

void MainWindow::ObtenerFinal(QString info){
    infoFinal.clear();
    infoFinal.append(info);
    GuardadoTemporal();
}

void MainWindow::on_BotonCerrarProyecto_clicked()
{
    int reply;
    QString texto;
    on_BotonMProyectoPulsado_clicked();
    reply = QMessageBox::question(this, tr("Salir"),
                                     tr("¿Realmente desea cerrar el proyecto sin guardarlo?\n(Perdera los cambios no guardados)"),
                                     tr("Salir sin guardar"), tr("Guardar y salir"), tr("Volver al proyecto"));

    if(reply == 0){
        estadoPieza = -2;
        ProyectoCargado(false);
        EjeCalculado(false);
        ui.tituloProyecto->setText("");
        texto.prepend("SIDRAC 2.0 - Sin proyecto ");
        setWindowTitle(texto);
        emit ui.glArea->CerrarProyecto();

    }else if(reply == 1){
        on_BotonGuardarProyecto_clicked();
        estadoPieza = -2;
        ProyectoCargado(false);
        EjeCalculado(false);
        ui.tituloProyecto->setText("");
        texto.prepend("SIDRAC 2.0 - Sin proyecto ");
        setWindowTitle(texto);
        emit ui.glArea->CerrarProyecto();
    }
}

void MainWindow::on_BotonCargarProyecto_clicked()
{
    float valor;
    float * vvalor;
    on_BotonMProyectoPulsado_clicked();
    QString contenidoArchivo;
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Cargar proyecto"), QDir::currentPath() + "/../save/",
                    tr("Proyecto SIDRAC 2 (*.sdc2)"));
    if(!fileName.isEmpty()){
        if(fileName.endsWith(".sdc2")){
            delete ficheroProyecto;
            ficheroProyecto = new QFile(fileName);
            ficheroProyecto->open(QIODevice::ReadWrite | QIODevice::Text);
            outProyecto = new QTextStream(ficheroProyecto);
            (*outProyecto) >> contenidoArchivo;
            if(contenidoArchivo == tr("%%SIDRAC2%%")){
                (*outProyecto) >> nombreProyecto;
                (*outProyecto) >> contenidoArchivo;
                estadoPieza = contenidoArchivo.toInt();
                if(estadoPieza >= -1){
                    if(estadoPieza >= 0){ //Se tiene que cargar pieza
                        (*outProyecto) >> contenidoArchivo;
                        if(contenidoArchivo.endsWith(".ply")){ //el archivo es de tipo ply
                            if(QFile::exists(contenidoArchivo)){        //el archivo de ply existe
                                emit loadMesh(contenidoArchivo,0);
                                ui.BotonMEje->setEnabled(true);
                                if(estadoPieza >= 1){ //se tiene que cargar eje
                                    ProyectoCargado(true);
                                    TituloProyecto(nombreProyecto);
                                    infoEje.clear();
                                    delete [] vvalor;
                                    vvalor = new float [8];
                                    for(int i=0; i<8;++i){
                                        (*outProyecto) >> contenidoArchivo;
                                        infoEje.append(contenidoArchivo);
                                        infoEje.append("\n");
                                        vvalor[i] = contenidoArchivo.toFloat();
                                    }
                                    emit ui.glArea->cargandoEje(vvalor);
                                    ui.BotonDibujarVoxels->setEnabled(false);
                                    ui.sliderVoxeles->setEnabled(false);
                                    ui.tituloSliderVoxeles->setEnabled(false);
                                    EjeCalculado(true);
                                    if(estadoPieza >= 2){
                                        infoContorno.clear();
                                        delete [] vvalor;
                                        (*outProyecto) >> contenidoArchivo;
                                        infoContorno.append(contenidoArchivo);
                                        infoContorno.append("\n");
                                        valor = contenidoArchivo.toFloat();
                                        int aux = (valor*2+1);
                                        vvalor = new float [aux];
                                        vvalor[0] = valor;
                                        for(int i=1; i<valor*2+1; i = i+2){
                                            (*outProyecto) >> contenidoArchivo;
                                            infoContorno.append(contenidoArchivo);
                                            infoContorno.append("\n");
                                            vvalor[i] = contenidoArchivo.toFloat();
                                            (*outProyecto) >> contenidoArchivo;
                                            infoContorno.append(contenidoArchivo);
                                            infoContorno.append("\n");
                                            vvalor[i+1] = contenidoArchivo.toFloat();
                                        }
                                        emit ui.glArea->cargandoContorno(vvalor);
                                        (*outProyecto) >> contenidoArchivo;
                                        infoContorno.append(contenidoArchivo);
                                        if(estadoPieza >= 3){
                                            infoFinal.clear();
                                            (*outProyecto) >> contenidoArchivo;
                                            infoFinal.append(contenidoArchivo);
                                            infoFinal.append("\n");
                                            delete [] vvalor;
                                            vvalor = new float [6];
                                            for(int i=0; i<6;++i){
                                                (*outProyecto) >> contenidoArchivo;
                                                infoFinal.append(contenidoArchivo);
                                                infoFinal.append("\n");
                                                vvalor[i] = contenidoArchivo.toFloat();
                                            }
                                            emit ui.glArea->cargandoFinal(vvalor);
                                        }
                                    }
                                }
                            }else //el archivo de ply no existe
                                ui.statusbar->showMessage(tr("No se encuentra el fichero ") + contenidoArchivo + " en el que estaba almacenado el modelo de la pieza");
                        }else //no es de tipo ply
                            ui.statusbar->showMessage(":::Error en el contenido del archivo (archivo ply erroneo):::" + contenidoArchivo);
                    }else{
                        ProyectoCargado(true);
                        TituloProyecto(nombreProyecto);
                    }
                }else
                    ui.statusbar->showMessage(":::Error en el contenido del archivo:::");
            }else //el archivo tiene mal la cabecera
                ui.statusbar->showMessage(":::Cabecera del archivo erronea:::");
        }else //el archivo no tiene bien la extension
            ui.statusbar->showMessage(":::Archivo no valido:::");
    }
}

void MainWindow::on_BotonNuevoProyecto_clicked()
{
    on_BotonMProyectoPulsado_clicked();
    if(estadoPieza > -2)
        on_BotonCerrarProyecto_clicked();
    //Introducir nombre del nuevo proyecto
    bool ok;
    QString texto = QInputDialog::getText(this,tr("Nombre del nuevo proyecto"),
                                          tr("Inserte nombre del proyecto:"), QLineEdit::Normal,
                                          "ejemplo", &ok);

    if(ok && !texto.isEmpty()){
        TituloProyecto(texto);
        ui.statusbar->showMessage("Proyecto creado");
        ProyectoCargado(true);
    }else{
        ui.statusbar->showMessage("Proyecto no creado");
    }
    estadoPieza = -1;
}

void MainWindow::on_BotonGuardarProyectoComo_clicked()
{
    guardadoAsociado = true;
    on_BotonMProyectoPulsado_clicked();
    QString fileName = QFileDialog::getSaveFileName(this,
                    tr("Guardar proyecto"), QDir::currentPath(),
                    tr("Proyecto SIDRAC2 (*.sdc2)"));

    if(!fileName.isEmpty()){
        delete ficheroProyecto;
        rutaDeGuardado.clear();
        rutaDeGuardado.append(fileName);
        ficheroProyecto = new QFile(fileName);
        on_BotonGuardarProyecto_clicked();
    }else{
        ui.statusbar->showMessage("Proyecto no guardado");
    }
}

void MainWindow::on_BotonGuardarProyecto_clicked()
{
    on_BotonMProyectoPulsado_clicked();
    if(guardadoAsociado){
        ficheroProyecto->open(QIODevice::ReadWrite | QIODevice::Text);
        outProyecto = new QTextStream(ficheroProyecto);


        (*outProyecto) << "%%SIDRAC2%%\n";
        (*outProyecto) << nombreProyecto << "\n";
        (*outProyecto) << estadoPieza << "\n"; // negativo si no hay pieza, 0 si recien cargada, 1 con eje calculado, 2 con contorno seleccionado, 3 con mas info
        if(estadoPieza >= 0){
            (*outProyecto) << nombrePieza << "\n";
            if(estadoPieza >= 1){
                //Solicitar informacion del eje
                (*outProyecto) << infoEje << "\n";
                if(estadoPieza >= 2){
                    //Solicitar informacion del contorno
                    (*outProyecto) << infoContorno << "\n";
                    if(estadoPieza >= 3){
                        //Solicitar informacion del contorno
                        (*outProyecto) << infoFinal << "\n";
                    }
                }
            }
        }

        outProyecto->flush();
        ficheroProyecto->close();
        ui.statusbar->showMessage("Proyecto guardado con éxito");
    }else{
        on_BotonGuardarProyectoComo_clicked();
    }
}

void MainWindow::GuardadoTemporal(){
    ficheroTemp->open(QIODevice::ReadWrite | QIODevice::Text);
    guardadoTemp = new QTextStream(ficheroTemp);
    (*guardadoTemp) << "%%SIDRAC2%%\n";
    (*guardadoTemp) << nombreProyecto << "\n";
    (*guardadoTemp) << estadoPieza << "\n"; // negativo si no hay pieza, 0 si recien cargada, 1 con eje calculado, 2 con contorno seleccionado, 3 con mas info
    if(guardadoTemp >= 0){
        (*guardadoTemp) << nombrePieza << "\n";
        if(estadoPieza >= 1){
            //Solicitar informacion del eje
            (*guardadoTemp) << infoEje << "\n";
            if(estadoPieza >= 2){
                //Solicitar informacion del contorno
                (*guardadoTemp) << infoContorno << "\n";
                if(estadoPieza >= 3){
                    //Solicitar informacion del contorno
                    (*guardadoTemp) << infoFinal << "\n";
                }
            }
        }
    }
    guardadoTemp->flush();
    ficheroTemp->close();
}

void MainWindow::on_BotonElementos_clicked()
{
    SetMenuModo(false);
    SetElementos(true);
}

void MainWindow::on_elementos3_pressed()
{
  emit ui.glArea->cambioModo(3);
}

void MainWindow::on_elementos5_pressed()
{
  emit ui.glArea->cambioModo(5);
}

void MainWindow::on_elementos1_pressed()
{
  emit ui.glArea->cambioModo(0);
}

void MainWindow::on_elementos2_pressed()
{
  emit ui.glArea->cambioModo(2);
}

void MainWindow::on_elementos4_pressed()
{
  emit ui.glArea->cambioModo(4);
}

void MainWindow::BloquearCalculos(bool valor){/*
    valor = !valor;
    ui.BotonMProyecto->setEnabled(valor);
    ui.BotonImportar->setEnabled(valor);
    ui.BotonMExportar->setEnabled(valor);
    ui.BotonMEje->setEnabled(valor);
    if(estadoPieza > 0){
        ui.BotonMContorno->setEnabled(valor);
        if(estadoPieza > 1){
            ui.BotonM3D->setEnabled(valor);
            if(estadoPieza > 2)
                ui.BotonMDibArq->setEnabled(valor);
        }
    }*/
}

void MainWindow::on_BotonCalcularEje_clicked()
{
    emit ui.glArea->calcularInicializarEje(ui.BotonMetodo->isChecked());
    BloquearCalculos(true);
    ui.BotonEje2->setEnabled(true);
    ui.BotonTemporal->setEnabled(true);
    on_BotonMEjePulsado_clicked();
}

void MainWindow::on_BotonContornoExterior_clicked()
{
    if(ui.BotonContorno->isChecked()){
        ui.BotonContorno->setChecked(false);
        emit ui.glArea->seleccionarInterior(false);
    }
    if(ui.BotonContornoExterior->isChecked()){
        ui.glArea->setCursor(QCursor(Qt::CrossCursor));
        emit ui.glArea->seleccionarExterior(true);
    }else{
        ui.glArea->setCursor(QCursor(Qt::SizeAllCursor));
        emit ui.glArea->seleccionarExterior(false);
    }
}

void MainWindow::on_BotonContorno_clicked()
{
    if(ui.BotonContornoExterior->isChecked()){
        ui.BotonContornoExterior->setChecked(false);
        emit ui.glArea->seleccionarExterior(false);
    }
    if(ui.BotonContorno->isChecked()){
        ui.glArea->setCursor(QCursor(Qt::CrossCursor));
        emit ui.glArea->seleccionarInterior(true);
    }else{
        ui.glArea->setCursor(QCursor(Qt::SizeAllCursor));
        emit ui.glArea->seleccionarInterior(false);
    }
}

void MainWindow::on_BotonColor_clicked()
{
    QColor color = QColorDialog::getColor(QColor(1,0,0),this);
    int colores[3];
    color.toRgb();
    colores[0] = color.red();
    colores[1] = color.green();
    colores[2] = color.blue();
    //emit ui.glArea->EnviarColor(colores);
}

void MainWindow::on_BotonDeshacer_clicked()
{
    emit ui.glArea->deshacerContorno();
}

void MainWindow::on_BotonExportarDibujo_clicked()
{
    on_BotonMExportarPulsado_clicked();
    QString fileName = QFileDialog::getSaveFileName(this,
                    tr("Exportar imagen"), QDir::currentPath(),
                    tr("Todas las imagenes (*.png *.bmp *.jpg);;Imagen png (*.png);;Imagen bmp (*bmp);;Imagen jpg (*.jpg)"));

    if(!fileName.isEmpty()){
        QFile * exportarImagen = new QFile(fileName);
        if(fileName.data()[fileName.size()-2] == 'n')
            emit ui.glArea->ExportarImagen(exportarImagen,"PNG");
        else if(fileName.data()[fileName.size()-2] == 'm')
            emit ui.glArea->ExportarImagen(exportarImagen,"BMP");
        else if(fileName.data()[fileName.size()-2] == 'p')
            emit ui.glArea->ExportarImagen(exportarImagen,"JPG");
        ui.statusbar->showMessage("Imagen exportada");
    }else{
        ui.statusbar->showMessage("Imagen no exportada");
    }
}

void MainWindow::ActivarReconstruccion(){
    ui.BotonExportar->setEnabled(true);
    ui.BotonM3D->setEnabled(true);
    ui.BotonMDibArq->setEnabled(true);
    ui.BotonMReconstruccion->setEnabled(false);
    ui.BotonMEje->setEnabled(false);
    ui.BotonMContorno->setEnabled(false);
    estadoPieza = 2;
    GuardadoTemporal();
}

void MainWindow::on_BotonMoverPieza_clicked()
{
    emit ui.glArea->MoverPieza(ui.BotonMoverPieza->isChecked());
}

void MainWindow::on_FijarPrograma_clicked()
{
    ui.FijarPrograma->setHidden(true);
    ui.FijarPrograma_2->setHidden(false);
}

void MainWindow::on_FijarPrograma_2_clicked()
{
    ui.FijarPrograma_2->setHidden(true);
    ui.FijarPrograma->setHidden(false);
    ui.FijarPrograma->setChecked(false);
}

void MainWindow::on_FijarConstruccion_clicked()
{
    ui.FijarConstruccion->setHidden(true);
    ui.FijarConstruccion_2->setHidden(false);
}

void MainWindow::on_FijarConstruccion_2_clicked()
{
    ui.FijarConstruccion_2->setHidden(true);
    ui.FijarConstruccion->setHidden(false);
    ui.FijarConstruccion->setChecked(false);
}

void MainWindow::on_FijarVisualizacion_2_clicked()
{
    ui.FijarVisualizacion_2->setHidden(true);
    ui.FijarVisualizacion->setHidden(false);
    ui.FijarVisualizacion->setChecked(false);
}

void MainWindow::on_FijarVisualizacion_clicked()
{
    ui.FijarVisualizacion->setHidden(true);
    ui.FijarVisualizacion_2->setHidden(false);
}
