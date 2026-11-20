#include "Representante.h"
#include <QBrush>

Representante::Representante(double posX_, double posY_, double ancho_, double alto_, double e_, int jugador_)
    : Bloque(posX_, posY_, ancho_, alto_, 0, e_, jugador_) {
    setBrush(QColor(90, 160, 90));
    texto->setText("J" + QString::number(jugador));
}

void Representante::recibirImpacto(double) {
    destruido = true;
    setBrush(Qt::red);
}
