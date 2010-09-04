/****************************************************************************
 ****************************************************************************/

#include <QtGui>
#include "mainwindow.h"

MainWindow::MainWindow (QWidget * parent):QMainWindow (parent)
{
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
  out = new QTextStream(salida);
  int i=0;
  while(QFile::exists("HistorigramaNormales"+QString::number(i)+".txt"))i++;
  salida2 = new QFile("HistorigramaNormales"+QString::number(0)+".txt");
  salida2->open(QIODevice::ReadWrite | QIODevice::Text);
  out2 = new QTextStream(salida2);

    //maximiza la ventana
  ui.setupUi (this);
  planoMostrado = false;
  pantallaCompleta = false;
  modoVisualizacionActual = 0;
  //connections
  grupoAlineamientoModo = new QActionGroup(this);
  ui.actionPuntos->setActionGroup(grupoAlineamientoModo);
  ui.actionAristas->setActionGroup(grupoAlineamientoModo);
  ui.actionAristas_ocultas->setActionGroup(grupoAlineamientoModo);
  ui.actionAristas_y_caras_planas->setActionGroup(grupoAlineamientoModo);
  ui.actionCaras_planas->setActionGroup(grupoAlineamientoModo);
  ui.actionCaras_suavizadas->setActionGroup(grupoAlineamientoModo);

  //from toolFrame to glArea through mainwindow
  connect (this, SIGNAL (loadMesh(QString,int)),
           ui.glArea, SLOT(loadMesh(QString,int)));
  connect (this, SIGNAL (saveMesh(QString,int)),
           ui.glArea, SLOT(saveMesh(QString,int)));

  connect (ui.glArea, SIGNAL(ajustarMaximoVoxeles(int)),
           this, SLOT(ajustarMaximoVoxeles(int)));

  //para el cambio de modo dentro de herramientas
  connect (this, SIGNAL (cambioModo(int)),
           ui.glArea, SLOT (cambioModo(int)));

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

  connect(this, SIGNAL (cancelarSalir()),
          ui.glArea, SLOT(cancelarSalir()));
  ui.MenuPrincipal->setHidden(true);
  ui.MenuVisualizacion->setHidden(true);
  ui.MenuInteractuar->setHidden(true);
  ui.barraProgreso->setHidden(true);
  ui.glContorno->setHidden(true);
}



void MainWindow::on_actionPuntos_activated()
{
  emit cambioModo(0);
}

void MainWindow::on_actionAristas_activated()
{
  emit cambioModo(1);
}

void MainWindow::on_actionAristas_y_caras_planas_activated()
{
  emit cambioModo(6);
}

void MainWindow::on_actionCaras_suavizadas_activated()
{
  emit cambioModo(4);
}

void MainWindow::on_actionCaras_planas_activated()
{
  emit cambioModo(5);
}

void MainWindow::on_actionAristas_ocultas_activated()
{
  emit cambioModo(3);
}

void MainWindow::on_actionAcerca_de_activated()
{
     QMessageBox::about(this, tr("Sidrac 2"),
                        tr("Proyecto fin de carrera\nRealizado por Juan Miguel Huertas Delgado.\nVersion 2.0.1"));
}

void MainWindow::on_actionAcerca_de_Qt_activated()
{
     QMessageBox::aboutQt(this);
}

void MainWindow::on_BotonSalir_clicked()
{
        int reply;
     reply = QMessageBox::question(this, tr("Salir"),
                                     tr("¿Realmente desea salir de la aplicación sin guardar?\nSi sale sin guardar perderá los cambios realizados."),
                                     tr("Salir sin guardar"), tr("Guardar y salir"), tr("Volver a la aplicacion"));
     if (reply == 0){
         salida->close();
         salida2->close();
         exit(0);
     }else if (reply == 1){
         if(on_BotonExportar_clicked()){
            salida->close();
            salida2->close();
            exit(0);
        }else
            cancelarSalir();
     }else
         emit cancelarSalir();
}

void MainWindow::on_BotonImportar_clicked()
{
    ui.glArea->setCursor(QCursor(Qt::SizeAllCursor));

    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Importar modelo"), QDir::currentPath(),
                    tr("Modelo ply (*.ply);;Modelo OBJ (*.3ds)"));
    if(!fileName.isEmpty()){
        if(fileName.endsWith(".ply"))
            emit loadMesh(fileName,0);
        else
            emit loadMesh(fileName,1);
        ui.BotonDibujar->setEnabled(true);
        ui.BotonDibujarEje->setEnabled(false);
        ui.BotonDibujarVoxels->setEnabled(false);
        ui.BotonNormales->setEnabled(true);
        parametrosEstimados = false;
    }

    if(ui.BotonGenerar->isChecked()){
        ui.BotonGenerar->setChecked(false);
        emit ui.glArea->Generar(ui.generarInferior->isChecked());
    }

}

bool MainWindow::on_BotonExportar_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                    tr("Exportar modelo"), QDir::currentPath(),
                    tr("Modelo ply (*.ply);;Modelo OBJ (*.3ds)"));
  if(!fileName.isEmpty()){
    if(fileName.endsWith(".ply"))
        emit saveMesh(fileName,0);
    else
        emit saveMesh(fileName,1);
    return true;
  }
  else return false;  
}
void MainWindow::mousePressEvent(QMouseEvent*e)
{
    /*
      int posy = e->x(), posx = e->y(), posx2 = height() - posx, posy2 = width() - posy;
      bool activado = false;
      if(ui.MenuPrincipal->isHidden()){
          if(posy <=40){
            emit ui.MenuPrincipal->setHidden(false);
            emit ui.etiquetaPrincipal->setHidden(true);
            activado = true;
          }
      }else{
          if(posy <= 160){
            emit ui.MenuPrincipal->setHidden(true);
            emit ui.etiquetaPrincipal->setHidden(false);
            activado = true;
              }
          }
      if(!activado){
          if(ui.MenuVisualizacion->isHidden()){
              if(posx2 > 30 && posx2 < 70){
                  ui.MenuVisualizacion->setHidden(false);
                  ui.etiquetaVisualizacion->setHidden(true);
                  activado = true;
              }
          }else{
              if(posx2 > 30 && posx2 < 190){
                  ui.MenuVisualizacion->setHidden(true);
                  ui.etiquetaVisualizacion->setHidden(false);
                  activado = true;
              }
          }
      }
      if(!activado){
          if(ui.MenuInteractuar->isHidden()){
              if(posy2 <= 40){
                  emit ui.MenuInteractuar->setHidden(false);
                  emit ui.etiquetaInteractuar->setHidden(true);
                  activado = true;
              }
          }else if(posy2 <=160){
                  emit ui.MenuInteractuar->setHidden(true);
                  emit ui.etiquetaInteractuar->setHidden(false);
                  activado = true;
              }
      }*/
 }


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
}

void MainWindow::on_BotonTemporal_clicked()
{
    emit ui.glArea->continuarRANSAC();
}

void MainWindow::on_BotonMostrarPlano_clicked()
{
    if(!planoMostrado){
        emit ui.glArea->dibujaPlano();
        ui.BotonMostrarPlano->setChecked(true);
    }else{
        emit ui.glArea->dibujaPlano();
        ui.BotonMostrarPlano->setChecked(false);
    }
    planoMostrado = !planoMostrado;
}

void MainWindow::on_BotonEje_clicked()
{
    if(parametrosEstimados)
        CrearConfiguracionEje(0);
    else{
        emit ui.glArea->calcularEje();
        parametrosEstimados = true;
    }
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
    //emit ui.glArea->reCalidadDelEje(ui.step->isChecked());
    emit ui.glArea->reCalidadDelEje(1);
}

void MainWindow::on_BotonDibujar_clicked()
{
    emit ui.glArea->cambiaModoDibujado();
}

void MainWindow::Imprimir(QString texto){
    (*out) << texto;
}

void MainWindow::ImprimirHistorigrama(QString texto){
    (*out2) << texto;
}

void MainWindow::on_BotonNormales_clicked()
{
    emit ui.glArea->dibujaNormales();
}

void MainWindow::on_BotonContorno_clicked()
{
    if(ui.BotonContorno->isChecked())
        ui.glArea->setCursor(QCursor(Qt::CrossCursor));
    else
        ui.glArea->setCursor(QCursor(Qt::SizeAllCursor));
    emit ui.glArea->seleccionar();
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
    ui.BotonContorno->setChecked(false);
    ui.glArea->setCursor(QCursor(Qt::SizeAllCursor));
    emit ui.glArea->seleccionar();
    emit ui.glArea->Generar(ui.generarInferior->isChecked());
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
    ui.generarInferior->setEnabled(true);
    ui.BotonDibujarVoxels->setEnabled(true);
    ui.sliderVoxeles->setEnabled(true);
    ui.tituloSliderVoxeles->setEnabled(true);
    emit ui.glArea->inicializarEje(limInf,limIntermedio,porcentaje,limiteIntersec,limiteIteracion,amplitud,amplitudMin,calcularVoxels,calcularEje,refinarEje);
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

void MainWindow::activaBotonEje(){
    ui.BotonDibujarEje->setEnabled(true);
}

void MainWindow::activaBotonContorno(){
    ui.BotonContorno->setEnabled(true);
}

void MainWindow::activaBotonVoxels(){
    ui.BotonDibujarVoxels->setEnabled(true);
    ui.sliderVoxeles->setEnabled(true);
}

void MainWindow::on_BotonInvertir_clicked()
{
    emit ui.glArea->invertirEje();
}
