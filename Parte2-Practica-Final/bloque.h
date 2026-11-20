#ifndef BLOQUE_H
#define BLOQUE_H

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include "Proyectil.h"

class Bloque : public QGraphicsRectItem {
protected:
    double posX;
    double posY;
    double ancho;
    double alto;
    double resistencia;
    double e;
    int jugador;
    bool destruido;
    QGraphicsSimpleTextItem* texto;

public:
    Bloque(double posX_, double posY_, double ancho_, double alto_, double resistencia_, double e_, int jugador_);

    bool resolverColision(Proyectil& p, double& velocidadImpacto);
    virtual void recibirImpacto(double danio);

    double getResistencia() const;
    int getJugador() const;
    bool estaDestruido() const;
    void ocultarTexto();
};

#endif