#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <QtGui/QDialog>

namespace Ui {
    class Configuracion;
}

class Configuracion : public QDialog {
    Q_OBJECT

public:
    Configuracion(QWidget *parent = 0, int * parametros = 0);
    ~Configuracion();

signals:
    void inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje);
    void obtenerPorDefecto();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Configuracion *m_ui;
    int limInfcopia;

private slots:
    void on_predeterminado_clicked();
    void on_calcularVoxels_clicked();
    void on_calcularEje_clicked();
    void on_buttonBox_accepted();

public slots:
    void porDefecto(int * parametrosDefecto);
};

#endif // CONFIGURACION_H
