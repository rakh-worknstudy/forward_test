#include "MathDriving.h"
#include <cmath>

Movement::Movement() : speed(0),
                       speedAngle(0),
                       accel(0),
                       accelAngle(0) {}
Movement::~Movement() {}

Car::Car(void) : position(0),
                 angle(0),
                 Movement() {
}
Car::~Car(void) {}

Road::Road(void) {}
Road::~Road(void) {}