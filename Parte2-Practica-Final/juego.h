#ifndef JUEGO_H
#define JUEGO_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QGraphicsLineItem>
#include <QPainterPath>
#include <QTimer>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include <QKeyEvent>
#include "Proyectil.h"
#include "Bloque.h"
#include "Representante.h"

class Juego : public QWidget {
    Q_OBJECT

private:
    QGraphicsScene* escena;
    QGraphicsView* vista;
    QTimer* timer;
    QSpinBox* spinAngulo;
    QSpinBox* spinVelocidad;
    QPushButton* botonLanzar;
    QPushButton* botonReiniciar;
    QLabel* etiquetaTurno;
    QLabel* etiquetaInfo;
    QLabel* etiquetaAyuda;

    Proyectil* proyectil;
    QGraphicsPathItem* trayectoria;
    QPainterPath camino;
    QVector<Bloque*> bloques;
    Representante* representantes[2];
    QGraphicsLineItem* canones[2];
    int anguloJugador[2];
    int velocidadJugador[2];
    int turno;
    double tiempoVuelo;

    void crearEscenario();
    void crearPlataforma(double x);
    void crearFortaleza(double xIzq, int jugador);
    void colisionesConBloques();
    void colisionesConParedes();
    void terminarTurno();
    void terminarJuego();
    void cargarControlesDelTurno();
    void habilitarControles(bool habilitar);
    void actualizarCanones();
    void puntaCanion(int jugador, double& x, double& y) const;

protected:
    void keyPressEvent(QKeyEvent* evento) override;

public:
    Juego(QWidget* parent = nullptr);

private slots:
    void lanzar();
    void actualizar();
    void reiniciar();
    void cambioControles();
};

#endif