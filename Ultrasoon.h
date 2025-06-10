#ifndef ULTRASOON_H
#define ULTRASOON_H

#include <util/delay.h>

void ultrasoon_setup();
void ultrasoon_setup_L();
void ultrasoon_setup_R();


enum enum_ultrasoon_sensorstypes{
    e_ultrasooonAchter,
    e_ultrasooonVoor,
    e_ultrasooonVolg
};
typedef enum enum_ultrasoon_sensorstypes t_ultrasoon_sensorstype;

void ultrasoon_switchSensor(t_ultrasoon_sensorstype sensor);

float ultrasoon_getDistance_L();
float ultrasoon_getDistance_R();

#endif // ULTRASOON_H
