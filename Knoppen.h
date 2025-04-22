#ifndef KNOPPEN_H
#define KNOPPEN_H

#include "Display1.h"
#include <avr/io.h>
#include <util/delay.h>

#define knopRegister DDRF
#define knopPins PINF

#define StartKnop PF0
#define OmlaagKnop PF2
#define OmhoogKnop PF1

extern int verwachtAantalPakketten;

void initKnop();
void displayGeteldAantalPakketten();
void stelWaardeIn();
void start();

#endif // KNOPPEN_H
