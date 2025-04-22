#ifndef DISPLAY_H
#define DISPLAY_H

extern unsigned char numbers [10];
extern unsigned char letters [7];

extern int teller;
extern int verwachtAantalPakketten;

void display(unsigned int getal);
void _7segment_setup();
void _7segment_write(unsigned char data, unsigned char digit);
void displayAantalPakketten(void);

#endif // DISPLAY_H
