#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <QGraphicsEllipseItem>

class Proyectil : public QGraphicsEllipseItem {
private:
    double vx;
    double vy;
    double masa;
    double radio;

public:
    Proyectil(double masa_, double radio_);

    void lanzar(double x0, double y0, double vx0, double vy0);
    void mover(double dt, double g);

    double getVx() const;
    double getVy() const;
    void setVelocidad(double vx_, double vy_);
    double getMasa() const;
    double getRadio() const;
};

#endif
