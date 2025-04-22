#include "Knoppen.h"
#include "Display1.h"

void initKnop(void) {
    knopRegister |= (1 << StartKnop) | (1 << OmlaagKnop) | (1 << OmhoogKnop);
    knopPins |= (1 << StartKnop) | (1 << OmlaagKnop) | (1 << OmhoogKnop);;
}

void stelWaardeIn() {
    while(((knopPins & (1 << StartKnop)))) {
        if((!(knopPins & (1 << OmlaagKnop))) && (verwachtAantalPakketten > 0)) {
            verwachtAantalPakketten--;
            while(!(knopPins & (1 << OmlaagKnop))) {
                _delay_ms(1);
                display(verwachtAantalPakketten);
            }
        } if (((!(knopPins & (1 << OmhoogKnop))) && (verwachtAantalPakketten < 99))) {
            verwachtAantalPakketten++;
            while(!(knopPins & (1 << OmhoogKnop))) {
                _delay_ms(1);
                display(verwachtAantalPakketten);
            }
        }
        display(verwachtAantalPakketten);
    }
    //return verwachtAantalPakketten;
}

void start() {
    if(!(knopPins & (1 << StartKnop))) {
        DDRB |= (1 << PB6);
        //return 1;
    } else {
        //return 0;
    }
}
