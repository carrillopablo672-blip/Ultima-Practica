#include "Bloque.h"
#include <QBrush>
#include <QPen>
#include <algorithm>
#include <cmath>

using namespace std;

Bloque::Bloque(double posX_, double posY_, double ancho_, double alto_, double resistencia_, double e_, int jugador_)
    : QGraphicsRectItem(posX_, posY_, ancho_, alto_),
    posX(posX_), posY(posY_), ancho(ancho_), alto(alto_),
    resistencia(resistencia_), e(e_), jugador(jugador_), destruido(false) {
    setPen(QPen(Qt::black, 0));
    setBrush(QColor(170, 120, 70));
    texto = new QGraphicsSimpleTextItem(QString::number(resistencia), this);
    texto->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    texto->setPos(posX + 0.3, posY + alto - 0.3);
}

bool Bloque::resolverColision(Proyectil& p, double& velocidadImpacto) {
    double r = p.getRadio();
    double qx = max(posX, min(p.x(), posX + ancho));
    double qy = max(posY, min(p.y(), posY + alto));
    double dx = p.x() - qx;
    double dy = p.y() - qy;
    double distancia = sqrt(dx * dx + dy * dy);

    if (distancia >= r || distancia == 0) return false;

    double nx = dx / distancia;
    double ny = dy / distancia;
    p.setPos(qx + nx * r, qy + ny * r);

    double vn = p.getVx() * nx + p.getVy() * ny;
    if (vn >= 0) return false;

    velocidadImpacto = sqrt(p.getVx() * p.getVx() + p.getVy() * p.getVy());

    double vPerpX = nx * vn;
    double vPerpY = ny * vn;
    double vParX = p.getVx() - vPerpX;
    double vParY = p.getVy() - vPerpY;
    p.setVelocidad(vParX - e * vPerpX, vParY - e * vPerpY);
    return true;
}

void Bloque::recibirImpacto(double danio) {
    resistencia -= danio;
    if (resistencia <= 0) {
        resistencia = 0;
        destruido = true;
        setVisible(false);
    }
    texto->setText(QString::number((int)ceil(resistencia)));
}

double Bloque::getResistencia() const { return resistencia; }
int Bloque::getJugador() const { return jugador; }
bool Bloque::estaDestruido() const { return destruido; }
void Bloque::ocultarTexto() { texto->setVisible(false); }