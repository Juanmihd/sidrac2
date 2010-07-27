/**
  * Autor: Juan Miguel Huertas Delgado
  * configuracion.cpp configuracion.h configuracion.ui (y los generados automaticamente por este)
  *
  * Estos archivos contienen lo necesario para establecer la configuracion con la que se realizaran
  * el calculo de los voxeles ("pixeles" en el espacio que representan el numero de intersecciones de las normales
  * en dicho espacio) y el calculo del eje (se utiliza un metodo probabilistico conocido como RANSAC)
  *
  */

#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <QtGui/QDialog>

namespace Ui {
    class Configuracion;
}

/**
  *
  * La clase Configuracion representa la ventana que se despliega a la hora de configurar el calculo
  * del eje y del proceso de obtencion del mismo. Tiene relacionado el archivo configuracion.ui en el
  * cual se ha diseñado la ventana.
  *
  * @secondary
  */

class Configuracion : public QDialog {
    Q_OBJECT

public:

    /**
      * Constructor que inicializa la clase de Configuracion y la ventana
      *
      * parent hace referencia al padre de esta clase, y parametros es un vector de enteros que contiene los parametros
      * que se le deben de pasar a la configuracion y que debe de mostrar en el momento de crearse.
      */
    Configuracion(QWidget *parent = 0, int * parametros = 0);

    /**
      * Destructor de la clase
      */
    ~Configuracion();

signals:
    /**
      * Señal que sirve para "exportar" todos los parametros incluidos en la configuracion, a saber:
      * limInf : el valor minimo de intersecciones que se tendra en cuenta a la hora de calcular los "voxeles"
      * limIntermedio : es el porcentaje de los voxeles calculados que se utilizaran a la hora de calcular el primer punto del eje
      * porcentaje : es el procentaje de los puntos que deben de recoger el eje, en el calculo del RANSAC
      * limiteIntersec : es el limite de intersecciones que se tienen en cuenta a la hora de calcular el primer punto del ransac, como maximo
      * limiteIteracion : es el limite de iteracioens a la hora de realizar RANSAC
      * amplitud : es la amplitud de espacio que abarca a la hora de calcular el RANSAC
      * amplitudMin : es la amplitud minima en la cual no se tomaran intersecciones de los voxeles
      * calcularVoxels : boolean que indica si se deben calcular los voxels o no
      * calcularEje : boolean que indica si se debe calcular RANSAC o no
      * refinarEje : boolean que indica que se va a refinar el eje (repetir RANSAC) o es la primera vez que se ejecuta
      *
      */
    void inicializarEje(int limInf, int limIntermedio, int porcentaje, int limiteIntersec, int limiteIteracion, int amplitud, int amplitudMin, bool calcularVoxels, bool calcularEje, bool refinarEje);
    /**
      * Es una señal que sirve para "importar" los valores por defecto que debe de tener el menu de configuracion
      */
    void obtenerPorDefecto();

protected:
    /**
      * Funcion interna para controlar el cambio de idioma
      */
    void changeEvent(QEvent *e);

private:
    // un puntero a la ventana de configuracion
    Ui::Configuracion *m_ui;
    // la copia del valor de limite inferior
    int limInfcopia;

private slots:
    /**
      * Funcion para cuando se pulsa el boton predeterminado: carga los valores predeterminados
      */
    void on_predeterminado_clicked();

    /**
      * Funcion para cuando se pulsa la casilla de "calcularVoxels": segun este activada o no
      * se pondran disponibles o no los parametros relacionados con calcular voxels
      */
    void on_calcularVoxels_clicked();

    /**
      * Funcion para cuando se pulsa la casilla de "calcularEje": segun este activada o no
      * se pondran disponibles o no los parametros relacionados con calcular eje
      */
    void on_calcularEje_clicked();

    /**
      * Funcion para controlar que ocurre cuando se pulsan los botones aceptar o cancelar. Si se pulsa
      * cancelar se cierra la ventana y no ocurre nada, simplemente quedaran alamcenados los parametros
      * que se han dejado puestos en la ventana. Si se pulsa aceptar se comenzara el calculo, en una hebra,
      * de lo que este seleccionado.
      */
    void on_buttonBox_accepted();

public slots:
    /**
      * Funcion relacionada con el signal obtenerPorDefecto() . Sirve para que cuando se calculen los valores
      * por defecto en la clase GLArea se llame a este slot y se sobreescriban los parametros aqui almacenados
      */
    void porDefecto(int * parametrosDefecto);
};

#endif // CONFIGURACION_H
