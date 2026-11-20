#include "Juego.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QtMath>
#include <cmath>

using namespace std;

const double ANCHO_CAJA = 100.0;
const double ALTO_CAJA = 50.0;
const double ESCALA = 10.0;
const double DT = 0.01;
const double G = 9.8;
const double TIEMPO_TURNO = 6.0;
const double E = 0.6;


const double FACTOR_DANIO = 1.5;
const double MASA_PROYECTIL = 2.0;
const double RADIO_PROYECTIL = 0.6;


const double ANCHO_PLATAFORMA = 4.0;
const double ALTO_PLATAFORMA = 0.5;
const double Y_PLATAFORMA = 12.0;


const double RADIO_BASE_CANION = 0.8;
const double LARGO_CANION = 2.0;
const double CANION_X[2] = {ANCHO_PLATAFORMA / 2, ANCHO_CAJA - ANCHO_PLATAFORMA / 2};
const double CANION_Y = Y_PLATAFORMA + ALTO_PLATAFORMA + RADIO_BASE_CANION;

const int ANGULO_INICIAL = 45;
const int VELOCIDAD_INICIAL = 25;

Juego::Juego(QWidget* parent) : QWidget(parent), turno(1), tiempoVuelo(0) {
    for (int i = 0; i < 2; i++) {
        anguloJugador[i] = ANGULO_INICIAL;
        velocidadJugador[i] = VELOCIDAD_INICIAL;
    }

    escena = new QGraphicsScene(-1, -1, ANCHO_CAJA + 2, ALTO_CAJA + 2, this);
    vista = new QGraphicsView(escena, this);
    vista->scale(ESCALA, -ESCALA);
    vista->setRenderHint(QPainter::Antialiasing);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setFixedSize((int)((ANCHO_CAJA + 4) * ESCALA), (int)((ALTO_CAJA + 4) * ESCALA));
    vista->setFocusPolicy(Qt::NoFocus);   // para que las flechas no muevan la vista

    escena->addRect(0, 0, ANCHO_CAJA, ALTO_CAJA, QPen(Qt::black, 0));

    // Cañones (tubo + base). El tubo se dibuja debajo de la base.
    for (int i = 0; i < 2; i++) {
        canones[i] = escena->addLine(QLineF(), QPen(Qt::darkGray, 0.5, Qt::SolidLine, Qt::RoundCap));
        canones[i]->setZValue(1);
        QGraphicsEllipseItem* base = escena->addEllipse(
            CANION_X[i] - RADIO_BASE_CANION, CANION_Y - RADIO_BASE_CANION,
            2 * RADIO_BASE_CANION, 2 * RADIO_BASE_CANION,
            QPen(Qt::black, 0), QBrush(QColor(60, 90, 140)));
        base->setZValue(2);
    }

    trayectoria = escena->addPath(QPainterPath(), QPen(Qt::blue, 0));
    trayectoria->setZValue(5);
    proyectil = new Proyectil(MASA_PROYECTIL, RADIO_PROYECTIL);
    proyectil->setZValue(10);
    escena->addItem(proyectil);

    crearEscenario();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Juego::actualizar);

    spinAngulo = new QSpinBox;
    spinAngulo->setRange(0, 90);
    spinAngulo->setValue(ANGULO_INICIAL);
    spinAngulo->setSuffix(" grados");
    spinAngulo->setFocusPolicy(Qt::NoFocus);

    spinVelocidad = new QSpinBox;
    spinVelocidad->setRange(10, 40);
    spinVelocidad->setValue(VELOCIDAD_INICIAL);
    spinVelocidad->setSuffix(" m/s");
    spinVelocidad->setFocusPolicy(Qt::NoFocus);

    connect(spinAngulo, QOverload<int>::of(&QSpinBox::valueChanged), this, &Juego::cambioControles);
    connect(spinVelocidad, QOverload<int>::of(&QSpinBox::valueChanged), this, &Juego::cambioControles);

    botonLanzar = new QPushButton("Lanzar");
    botonLanzar->setFocusPolicy(Qt::NoFocus);
    connect(botonLanzar, &QPushButton::clicked, this, &Juego::lanzar);

    botonReiniciar = new QPushButton("Reiniciar");
    botonReiniciar->setFocusPolicy(Qt::NoFocus);
    connect(botonReiniciar, &QPushButton::clicked, this, &Juego::reiniciar);

    etiquetaTurno = new QLabel("Turno: Jugador 1");
    etiquetaInfo = new QLabel("");
    etiquetaAyuda = new QLabel("Teclado:  ↑/↓ ángulo   ←/→ velocidad   Espacio lanzar   R reiniciar");

    QHBoxLayout* controles = new QHBoxLayout;
    controles->addWidget(etiquetaTurno);
    controles->addWidget(new QLabel("Ángulo:"));
    controles->addWidget(spinAngulo);
    controles->addWidget(new QLabel("Velocidad:"));
    controles->addWidget(spinVelocidad);
    controles->addWidget(botonLanzar);
    controles->addWidget(botonReiniciar);

    QVBoxLayout* principal = new QVBoxLayout(this);
    principal->addWidget(vista);
    principal->addLayout(controles);
    principal->addWidget(etiquetaInfo);
    principal->addWidget(etiquetaAyuda);

    setFocusPolicy(Qt::StrongFocus);
    setWindowTitle("Juego de estrategia por turnos");
    actualizarCanones();
}

void Juego::crearEscenario() {
    crearPlataforma(0);
    crearPlataforma(ANCHO_CAJA - ANCHO_PLATAFORMA);
    crearFortaleza(6, 1);
    crearFortaleza(82, 2);
}

void Juego::crearPlataforma(double x) {
    // Jugador 0 = neutral: no recibe daño. e = 1 porque es parte de la caja.
    Bloque* plataforma = new Bloque(x, Y_PLATAFORMA, ANCHO_PLATAFORMA, ALTO_PLATAFORMA, 0, 1.0, 0);
    plataforma->setBrush(Qt::white);
    plataforma->ocultarTexto();
    escena->addItem(plataforma);
    bloques.push_back(plataforma);
}

void Juego::crearFortaleza(double xIzq, int jugador) {
    representantes[jugador - 1] = new Representante(xIzq + 4, 0, 4, 5, E, jugador);

    Bloque* partes[4] = {
        new Bloque(xIzq, 0, 3, 8, 200, E, jugador),
        new Bloque(xIzq + 9, 0, 3, 8, 200, E, jugador),
        new Bloque(xIzq, 8, 12, 2, 100, E, jugador),
        representantes[jugador - 1]
    };

    for (Bloque* b : partes) {
        escena->addItem(b);
        bloques.push_back(b);
    }
}

void Juego::puntaCanion(int jugador, double& x, double& y) const {
    double angulo = qDegreesToRadians((double)anguloJugador[jugador - 1]);
    double direccion = (jugador == 1) ? 1 : -1;
    x = CANION_X[jugador - 1] + direccion * LARGO_CANION * cos(angulo);
    y = CANION_Y + LARGO_CANION * sin(angulo);
}

void Juego::actualizarCanones() {
    for (int j = 1; j <= 2; j++) {
        double xPunta, yPunta;
        puntaCanion(j, xPunta, yPunta);
        canones[j - 1]->setLine(CANION_X[j - 1], CANION_Y, xPunta, yPunta);

        QPen pen = canones[j - 1]->pen();
        if (j == turno) pen.setColor(turno == 1 ? Qt::blue : Qt::red);
        else pen.setColor(Qt::darkGray);
        canones[j - 1]->setPen(pen);
    }
}

void Juego::cambioControles() {
    anguloJugador[turno - 1] = spinAngulo->value();
    velocidadJugador[turno - 1] = spinVelocidad->value();
    actualizarCanones();
}

void Juego::cargarControlesDelTurno() {
    spinAngulo->blockSignals(true);
    spinVelocidad->blockSignals(true);
    spinAngulo->setValue(anguloJugador[turno - 1]);
    spinVelocidad->setValue(velocidadJugador[turno - 1]);
    spinAngulo->blockSignals(false);
    spinVelocidad->blockSignals(false);
    actualizarCanones();
}

void Juego::habilitarControles(bool habilitar) {
    spinAngulo->setEnabled(habilitar);
    spinVelocidad->setEnabled(habilitar);
    botonLanzar->setEnabled(habilitar);
}

void Juego::lanzar() {
    if (!botonLanzar->isEnabled()) return;   // evita lanzar dos veces con el teclado

    double angulo = qDegreesToRadians((double)anguloJugador[turno - 1]);
    double velocidad = velocidadJugador[turno - 1];
    double direccion = (turno == 1) ? 1 : -1;

    double x0, y0;
    puntaCanion(turno, x0, y0);   // sale desde la boca del cañón

    proyectil->lanzar(x0, y0, direccion * velocidad * cos(angulo), velocidad * sin(angulo));

    camino = QPainterPath(QPointF(x0, y0));
    trayectoria->setPath(camino);
    trayectoria->setPen(QPen(turno == 1 ? Qt::blue : Qt::red, 0));

    tiempoVuelo = 0;
    habilitarControles(false);
    etiquetaInfo->setText("");
    timer->start(10);
}

void Juego::actualizar() {
    tiempoVuelo += DT;
    proyectil->mover(DT, G);
    colisionesConBloques();
    colisionesConParedes();

    camino.lineTo(proyectil->pos());
    trayectoria->setPath(camino);

    int rival = 3 - turno;
    if (representantes[rival - 1]->estaDestruido()) terminarJuego();
    else if (tiempoVuelo >= TIEMPO_TURNO) terminarTurno();
}

void Juego::colisionesConBloques() {
    for (Bloque* b : bloques) {
        if (b->estaDestruido()) continue;

        double velocidadImpacto;
        if (!b->resolverColision(*proyectil, velocidadImpacto)) continue;
        if (b->getJugador() == 0 || b->getJugador() == turno) continue;   // plataformas y bloques propios no reciben daño

        double danio = FACTOR_DANIO * proyectil->getMasa() * velocidadImpacto;
        b->recibirImpacto(danio);

        etiquetaInfo->setText("Impacto a " + QString::number(velocidadImpacto, 'f', 1) + " m/s"
                              + "  ->  daño " + QString::number(danio, 'f', 1)
                              + "  ->  resistencia restante " + QString::number(b->getResistencia(), 'f', 1));
    }
}

void Juego::colisionesConParedes() {
    double x = proyectil->x(), y = proyectil->y();
    double vx = proyectil->getVx(), vy = proyectil->getVy();
    double r = proyectil->getRadio();

    if (x - r < 0 && vx < 0)          { vx = -vx; x = r; }
    if (x + r > ANCHO_CAJA && vx > 0) { vx = -vx; x = ANCHO_CAJA - r; }
    if (y - r < 0 && vy < 0)          { vy = -vy; y = r; }
    if (y + r > ALTO_CAJA && vy > 0)  { vy = -vy; y = ALTO_CAJA - r; }

    proyectil->setPos(x, y);
    proyectil->setVelocidad(vx, vy);
}

void Juego::terminarTurno() {
    timer->stop();
    proyectil->setVisible(false);
    turno = 3 - turno;
    etiquetaTurno->setText("Turno: Jugador " + QString::number(turno));
    cargarControlesDelTurno();
    habilitarControles(true);
}

void Juego::terminarJuego() {
    timer->stop();
    etiquetaTurno->setText("¡Gana el Jugador " + QString::number(turno) + "!");
    etiquetaInfo->setText("El representante del Jugador " + QString::number(3 - turno) + " fue alcanzado. Pulsa Reiniciar (R) para jugar otra vez.");
}

void Juego::reiniciar() {
    timer->stop();

    for (Bloque* b : bloques) delete b;   // al borrarlo, Qt lo quita de la escena
    bloques.clear();
    crearEscenario();

    turno = 1;
    tiempoVuelo = 0;
    for (int i = 0; i < 2; i++) {
        anguloJugador[i] = ANGULO_INICIAL;
        velocidadJugador[i] = VELOCIDAD_INICIAL;
    }

    proyectil->setVisible(false);
    camino = QPainterPath();
    trayectoria->setPath(camino);

    etiquetaTurno->setText("Turno: Jugador 1");
    etiquetaInfo->setText("");
    cargarControlesDelTurno();
    habilitarControles(true);
    setFocus();
}

void Juego::keyPressEvent(QKeyEvent* evento) {
    bool puedeApuntar = botonLanzar->isEnabled();

    switch (evento->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        if (puedeApuntar) spinAngulo->stepUp();
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        if (puedeApuntar) spinAngulo->stepDown();
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        if (puedeApuntar) spinVelocidad->stepUp();
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        if (puedeApuntar) spinVelocidad->stepDown();
        break;
    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        lanzar();
        break;
    case Qt::Key_R:
        reiniciar();
        break;
    default:
        QWidget::keyPressEvent(evento);
    }
}