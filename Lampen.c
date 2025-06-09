#include "lampjes.h"

#include <avr/io.h>

#define LAMPREGISTER DDRB
#define LAMPPORT PORTB
#define LAMPGROEN PB6
#define LAMPGEEL PB5

static uint8_t lampStatus = 0; // bit 0 = geel, bit 1 = groen

void initLamp() {
    LAMPREGISTER |= (1<<LAMPGEEL) | (1<<LAMPGROEN); // Set als output
    LAMPPORT &= ~((1<<LAMPGEEL) | (1<<LAMPGROEN));  // Alles uit
    lampStatus = 0;
}

void lampjesSet(t_lamp kleur, int aan) {
    if (kleur == GEEL) {
        if (aan) {
            LAMPPORT |= (1<<LAMPGEEL);
            lampStatus |= (1<<0);
        } else {
            LAMPPORT &= ~(1<<LAMPGEEL);
            lampStatus &= ~(1<<0);
        }
    } else if (kleur == GROEN) {
        if (aan) {
            LAMPPORT |= (1<<LAMPGROEN);
            lampStatus |= (1<<1);
        } else {
            LAMPPORT &= ~(1<<LAMPGROEN);
            lampStatus &= ~(1<<1);
        }
    }
}

int lampjesStatus(t_lamp kleur) {
    if (kleur == GEEL) {
        return (lampStatus & (1<<0)) != 0;
    } else if (kleur == GROEN) {
        return (lampStatus & (1<<1)) != 0;
    }
    return 0;
}
