#include "clock.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Display1.h"

#define FREQUENCY 1000
#define OVF 250

volatile float time = 0;

ISR(TIMER0_COMPA_vect){
    time+=1.0/FREQUENCY;
}


void initClock(){
    time = 0;

    //OVF
    OCR0A = OVF;

    //prescalar 1
    TCCR0B |= (1<<CS00)|(1<<CS01);

    //WGM CTC mode
    TCCR0A |= 1<<WGM01;

    //OVF interrupt
    TIMSK0 |= 1<<OCIE0A;

    sei();
}
