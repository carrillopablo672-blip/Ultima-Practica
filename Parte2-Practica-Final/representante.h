#ifndef REPRESENTANTE_H
#define REPRESENTANTE_H

#include "Bloque.h"

class Representante : public Bloque {
public:
    Representante(double posX_, double posY_, double ancho_, double alto_, double e_, int jugador_);

    void recibirImpacto(double danio) override;
};

#endif
