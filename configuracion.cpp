#include "configuracion.h"
#include "ui_configuracion.h"

Configuracion::Configuracion(QWidget *parent, int * parametros) :
    QDialog(parent),
    m_ui(new Ui::Configuracion)
{
    connect (this, SIGNAL (inicializarEje(int,int,int,int,int,int,int,bool,bool)),
           parent, SLOT (inicializarEje(int,int,int,int,int,int,int,bool,bool)));
    connect (this, SIGNAL (obtenerPorDefecto()),
             parent, SLOT (obtenerPorDefecto()));

    m_ui->setupUi(this);

    if(parametros != 0){
        m_ui->limiteInferior->setValue(parametros[0]);
        m_ui->limiteMedio->setValue(parametros[1]);
        m_ui->porcentaje->setValue(parametros[2]);
        m_ui->limiteIntersec->setValue(parametros[3]);
        m_ui->limiteIteraciones->setValue(parametros[4]);
        m_ui->amplitud->setValue(parametros[5]);
        m_ui->amplitudMin->setValue(parametros[6]);
        if(parametros[7])
            m_ui->calcularVoxels->setChecked(true);
        if(parametros[8])
            m_ui->calcularEje->setChecked(true);
    }
}

Configuracion::~Configuracion()
{
    delete m_ui;
}

void Configuracion::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Configuracion::on_buttonBox_accepted()
{
    limInfcopia = m_ui->limiteInferior->text().toInt();
    emit inicializarEje(m_ui->limiteInferior->text().toInt(),
                        m_ui->limiteMedio->text().toInt(),
                        m_ui->porcentaje->text().toInt(),
                        m_ui->limiteIntersec->text().toInt(),
                        m_ui->limiteIteraciones->text().toInt(),
                        m_ui->amplitud->text().toInt(),
                        m_ui->amplitudMin->text().toInt(),
                        m_ui->calcularVoxels->isChecked(),
                        m_ui->calcularEje->isChecked());
}

void Configuracion::porDefecto(int * parametrosDefecto){
    m_ui->limiteInferior->setValue(parametrosDefecto[0]);
    m_ui->limiteMedio->setValue(parametrosDefecto[1]);
    m_ui->porcentaje->setValue(parametrosDefecto[2]);
    m_ui->limiteIntersec->setValue(parametrosDefecto[3]);
    m_ui->limiteIteraciones->setValue(parametrosDefecto[4]);
    m_ui->amplitud->setValue(parametrosDefecto[5]);
    m_ui->amplitudMin->setValue(parametrosDefecto[6]);
    m_ui->calcularVoxels->setChecked(true);
    m_ui->calcularEje->setChecked(true);
}

void Configuracion::on_calcularEje_clicked()
{
    m_ui->porcentaje->setEnabled(m_ui->calcularEje->isChecked());
    m_ui->limiteIntersec->setEnabled(m_ui->calcularEje->isChecked());
    m_ui->limiteIteraciones->setEnabled(m_ui->calcularEje->isChecked());
    m_ui->amplitud->setEnabled(m_ui->calcularEje->isChecked());
}

void Configuracion::on_calcularVoxels_clicked()
{
    m_ui->limiteInferior->setEnabled(m_ui->calcularVoxels->isChecked());
    m_ui->limiteMedio->setEnabled(m_ui->calcularVoxels->isChecked());
    m_ui->amplitudMin->setEnabled(m_ui->calcularVoxels->isChecked());
}

void Configuracion::on_predeterminado_clicked()
{
    emit obtenerPorDefecto();
}
