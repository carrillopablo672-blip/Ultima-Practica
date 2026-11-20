#include "Proyectil.h"
#include <QBrush>
#include <QPen>

Proyectil::Proyectil(double masa_, double radio_)
    : QGraphicsEllipseItem(-radio_, -radio_, 2 * radio_, 2 * radio_),
    vx(0), vy(0), masa(masa_), radio(radio_) {
    setPen(QPen(Qt::black, 0));
    setBrush(Qt::black);
    setVisible(false);
}

void Proyectil::lanzar(double x0, double y0, double vx0, double vy0) {
    setPos(x0, y0);
    vx = vx0;
    vy = vy0;
    setVisible(true);
}

void Proyectil::mover(double dt, double g) {
    double nuevaX = x() + vx * dt;
    double nuevaY = y() + vy * dt - 0.5 * g * dt * dt;
    vy = vy - g * dt;
    setPos(nuevaX, nuevaY);
}

double Proyectil::getVx() const { return vx; }
double Proyectil::getVy() const { return vy; }

void Proyectil::setVelocidad(double vx_, double vy_) {
    vx = vx_;
    vy = vy_;
}

double Proyectil::getMasa() const { return masa; }
double Proyectil::getRadio() const { return radio; }
