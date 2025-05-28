#ifndef KNOPPEN_H
#define KNOPPEN_H

enum enum_knoppen{e_startKnop, e_limitSwitch};
typedef enum enum_knoppen t_knoppen;

void knop_setup();
int knop_ingedrukt(t_knoppen knop);

#endif // KNOPPEN_H
