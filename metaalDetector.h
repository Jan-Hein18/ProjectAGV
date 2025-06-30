#ifndef METAALDETECTOR_INCLUDED
#define METAALDETECTOR_INCLUDED

void metaaldetector_setup();

extern volatile long int ICRL;
extern volatile long int ICRR;
extern volatile int metaalLinks;
extern volatile int metaalRechts;
extern volatile unsigned long int setPointR;

int metaaldetector_links();
int metaaldetector_rechts();

void metaaldetector_setPointL();
void metaaldetector_setPointR();

#endif // METAALDETECTOR_INCLUDED
