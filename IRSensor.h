#ifndef IRSENSOR_H
#define IRSENSOR_H


#include <avr/io.h>
#include <util/delay.h>

#define IRRegister DDRK
#define IRPins PINK
#define IRSensor1 PK0
#define IRSensor2 PK1

extern int aantalPakketten;
extern int pakketGeteld;

void initSensoren(void);
int detecteerPakket0(void);
int detecteerPakket1(void);
int telPakketten(void);

#endif //IRSENSOR_H
