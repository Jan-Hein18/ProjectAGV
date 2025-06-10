#include "Ultrasoon.h"
#include "avr/io.h"
#include "avr/interrupt.h"

#define ULTRASOON_L_TRIG_BIT PL4  // OC5B
#define ULTRASOON_L_TRIG_DDR DDRL
#define ULTRASOON_L_ECHO_BIT PL1  // ICP5
#define ULTRASOON_L_ECHO_DDR DDRL

#define ULTRASOON_R_TRIG_BIT PH4  // OC4B
#define ULTRASOON_R_TRIG_DDR DDRH
#define ULTRASOON_R_ECHO_BIT PL0  // ICP4
#define ULTRASOON_R_ECHO_DDR DDRL

#define PRESCALAR 64

#define ULTRASOON_MAXDISTANCE 30


volatile uint8_t nieuwe_meting_L = 0;
volatile uint8_t nieuwe_meting_R = 0;


// ISR voor linker sensor (echo ontvangen)
ISR(TIMER5_CAPT_vect) {
    //start nieuwe meting
    TCNT5 = 0;

    nieuwe_meting_L = 1;
}

// ISR voor rechter sensor (echo ontvangen)
ISR(TIMER4_CAPT_vect) {
    //start nieuwe meting
    TCNT4 = 0;

    nieuwe_meting_R = 1;
}

void ultrasoon_setup() {
    ultrasoon_setup_L();
    ultrasoon_setup_R();
    sei();  // Schakel interrupts in
}

void ultrasoon_setup_L() {
    TCCR5B &= ~((1 << CS50) | (1 << CS51));//stop clock
    ICR5 = 0;
    nieuwe_meting_L = 0;

    ULTRASOON_L_TRIG_DDR |= (1 << ULTRASOON_L_TRIG_BIT);  // Trigger als output
    ULTRASOON_L_ECHO_DDR &= ~(1 << ULTRASOON_L_ECHO_BIT); // Echo als input

    // Timer 5 instellen voor 10 bit PWM en input capture
    TCCR5A |= (1 << WGM50) | (1 << WGM51);
    TCCR5B |= (1 << WGM52) | (0 << WGM53);
    TCCR5A |= (1 << COM5B1);
    TCCR5B |= (1 << CS50) | (1 << CS51);

    //zet output uit
    OCR5A = 0;      // 12us puls = 3
    OCR5B = 0;      // 12us puls = 3
    OCR5C = 0;      // 12us puls = 3

    ultrasoon_switchSensor(e_ultrasooonVoor);

    // Input capture instellen
    TCCR5B &= ~(1 << ICES5); // Start op vallende flank
    TIMSK5 |= (1 << ICIE5);  // Interrupt inschakelen
}

void ultrasoon_setup_R() {
    TCCR4B &= ~((1 << CS40) | (1 << CS41));//stop clock
    ICR4 = 0;
    nieuwe_meting_R = 0;

    ULTRASOON_R_TRIG_DDR |= (1 << ULTRASOON_R_TRIG_BIT);
    ULTRASOON_R_ECHO_DDR &= ~(1 << ULTRASOON_R_ECHO_BIT);

    //10 bit pwm
    TCCR4A |= (1 << WGM40) | (1 << WGM41);
    TCCR4B |= (1 << WGM42) | (0 << WGM43);
    TCCR4A |= (1 << COM4B1);
    TCCR4B |= (1 << CS40) | (1 << CS41);

    OCR4A = 0;
    OCR4B = 0;
    OCR4C = 0;

    ultrasoon_switchSensor(e_ultrasooonVoor);

    TCCR4B &= ~(1 << ICES4);
    TIMSK4 |= (1 << ICIE4);
}


void ultrasoon_switchSensor(t_ultrasoon_sensorstype sensor){
    OCR4A = 0;
    OCR4B = 0;
    OCR4C = 0;

    OCR5A = 0;
    OCR5B = 0;
    OCR5C = 0;


    switch(sensor){ //12us pulse
    case e_ultrasooonAchter:{
        OCR4A = 3;
        OCR5A = 3;
    }
    case e_ultrasooonVoor:{
        OCR4B = 3;
        OCR5B = 3;
    }
    case e_ultrasooonVolg:{
        OCR4C = 3;
        OCR5C = 3;
    }
    }
}


float ultrasoon_getDistance_L() {
    static float afstand_in_cm = 0;

    if(nieuwe_meting_L){//alleen berekenen als de meetwaarde veranderd is
        nieuwe_meting_L = 0;
        unsigned long icrVal = ICR5;  // Use long to prevent overflow
        if(icrVal>550){
            unsigned long pulseTime_us = (((icrVal - 550) * PRESCALAR) / 16);
            afstand_in_cm = pulseTime_us / 58.0;
        }

        if (afstand_in_cm > ULTRASOON_MAXDISTANCE) {  // Fix stability issue at long distances
            afstand_in_cm = ULTRASOON_MAXDISTANCE;
        }
    }

    return afstand_in_cm;
}




float ultrasoon_getDistance_R() {
    static float afstand_in_cm = 0;

    if(nieuwe_meting_R){//alleen berekenen als de meetwaarde veranderd is
        nieuwe_meting_R = 0;
        unsigned long icrVal = ICR4;  // Use long to prevent overflow
        if(icrVal>550){
            unsigned long pulseTime_us = (((icrVal - 550) * PRESCALAR) / 16);
            afstand_in_cm = pulseTime_us / 58.0;
        }


        if (afstand_in_cm > ULTRASOON_MAXDISTANCE) {  // Fix stability issue at long distances
            afstand_in_cm = ULTRASOON_MAXDISTANCE;
        }
    }

    return afstand_in_cm ;
}

