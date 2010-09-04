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

****************************************************************************/
#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "ui_mainwindow.h"
#include "configuracion.h"
#include <QFile>
#include <QTextStream>


class MainWindow:public QMainWindow
{
Q_OBJECT 
public:
  MainWindow(QWidget * parent = 0);
  Configuracion *configuracionEje;
signals:
  void loadMesh(QString newMesh, int tipoArchivo);
  void saveMesh(QString newMesh, int tipoArchivo);
  void cambioModo(int modo);
  void cancelarSalir();
private:
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
  //Eventos de raton
  void mousePressEvent(QMouseEvent*e);
  void mouseMoveEvent(QMouseEvent*e);
  void mouseReleaseEvent(QMouseEvent*e);
  void wheelEvent(QWheelEvent*e);
  QFile * salida;
  QFile * salida2;
  QTextStream * out;
  QTextStream * out2;
private slots:
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
    void on_BotonMostrarPlano_clicked();
    void on_BotonTemporal_clicked();
    void on_BotonPantallaCompleta_clicked();
    bool on_BotonExportar_clicked();
    void on_BotonImportar_clicked();
    void on_BotonSalir_clicked();
    void on_actionAcerca_de_Qt_activated();
    void on_actionAcerca_de_activated();
    void on_actionAristas_ocultas_activated();
    void on_actionCaras_planas_activated();
    void on_actionCaras_suavizadas_activated();
    void on_actionAristas_y_caras_planas_activated();
    void on_actionAristas_activated();
    void on_actionPuntos_activated();
    void Imprimir(QString texto);
    void CrearConfiguracionEje(int * parametros);
    void ImprimirHistorigrama(QString texto);
    void IniciarBarra();
    void AvanzarBarra(int valor);
    void AcabarBarra();
    void obtenerPorDefecto();
    void inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje, bool refinarEje);
};

#endif /*MAINWINDOW_H_ */
