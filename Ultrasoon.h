#ifndef ULTRASOON_H
#define ULTRASOON_H

#include <util/delay.h>

void ultrasoon_setup();
void ultrasoon_setup_L();
void ultrasoon_setup_R();

float ultrasoon_getDistance_L();
float ultrasoon_getDistance_R();

#endif // ULTRASOON_H
