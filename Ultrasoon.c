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

#define max_stable_distance 30

//volatile uint8_t meting_klaar_L = 1;  // Start klaar voor eerste meting
//volatile uint8_t meting_klaar_R = 1;

volatile uint8_t nieuwe_meting_L = 0;
volatile uint8_t nieuwe_meting_R = 0;


// ISR voor linker sensor (echo ontvangen)
ISR(TIMER5_CAPT_vect) {
    //start nieuwe meting
    TCNT5 = 0;

    //meting_klaar_L = 1;  // Markeer meting als voltooid
    nieuwe_meting_L = 1;
}

// ISR voor rechter sensor (echo ontvangen)
ISR(TIMER4_CAPT_vect) {
    //start nieuwe meting
    TCNT5 = 0;

    //meting_klaar_R = 1;
    nieuwe_meting_R = 1;
}

void ultrasoon_setup() {
    ultrasoon_setup_L();
    ultrasoon_setup_R();
    sei();  // Schakel interrupts in
}

void ultrasoon_setup_L() {
    ULTRASOON_L_TRIG_DDR |= (1 << ULTRASOON_L_TRIG_BIT);  // Trigger als output
    ULTRASOON_L_ECHO_DDR &= ~(1 << ULTRASOON_L_ECHO_BIT); // Echo als input

    // Timer 5 instellen voor PWM en input capture
    TCCR5A |= (1 << WGM50) | (1 << WGM51);
    TCCR5B |= (1 << WGM52) | (1 << WGM53);
    TCCR5A |= (1 << COM5B1);
    TCCR5B |= (1 << CS50) | (1 << CS51);

    OCR5A = 15000;  // 60ms periode
    OCR5B = 3;      // 12us puls

    // Input capture instellen
    TCCR5B &= ~(1 << ICES5); // Start op vallende flank
    TIMSK5 |= (1 << ICIE5);  // Interrupt inschakelen
}

void ultrasoon_setup_R() {
    ULTRASOON_R_TRIG_DDR |= (1 << ULTRASOON_R_TRIG_BIT);
    ULTRASOON_R_ECHO_DDR &= ~(1 << ULTRASOON_R_ECHO_BIT);

    TCCR4A |= (1 << WGM40) | (1 << WGM41);
    TCCR4B |= (1 << WGM42) | (1 << WGM43);
    TCCR4A |= (1 << COM4B1);
    TCCR4B |= (1 << CS40) | (1 << CS41);

    OCR4A = 15000;
    OCR4B = 3;

    TCCR4B &= ~(1 << ICES4);
    TIMSK4 |= (1 << ICIE4);
}

/*void ultrasoon_trigger_L() {
    if (meting_klaar_L) {  // Alleen nieuwe meting starten als de vorige klaar is
        meting_klaar_L = 0;  // Reset vlag
        ULTRASOON_L_TRIG_DDR |= (1 << ULTRASOON_L_TRIG_BIT);
        TCNT5 = 0;  // Timer resetten
    }
}

void ultrasoon_trigger_R() {
    if (meting_klaar_R) {
        meting_klaar_R = 0;
        ULTRASOON_R_TRIG_DDR |= (1 << ULTRASOON_R_TRIG_BIT);
        TCNT4 = 0;
    }
}*/

float ultrasoon_getDistance_L() {
    /*static float last_valid_distance = 30.0;
    static int zero_count = 0;*/

    static float afstand_in_cm = 0;

    if(nieuwe_meting_L){//alleen berekenen als de meetwaarde veranderd is
        nieuwe_meting_L = 0;
        unsigned long icrVal = ICR5;  // Use long to prevent overflow
        if(icrVal>550){
            unsigned long pulseTime_us = (((icrVal - 550) * PRESCALAR) / 16);
            afstand_in_cm = pulseTime_us / 58.0;
        }

        if (afstand_in_cm > max_stable_distance) {  // Fix stability issue at long distances
            afstand_in_cm = max_stable_distance;
        }
    }


    //Dit kan (bijna) niet als je hier nog wat mee wil voor de bochten zou ik dit doen met afstand > 30 in de main, of 4 regels hierboven
    /*if (afstand_in_cm == 0) {
        zero_count++;
        if (zero_count >= 3) {
            return 0;  // Confirmed "wall lost"
        }
        return last_valid_distance;
    } else {
        zero_count = 0;
    }

    last_valid_distance = (last_valid_distance * 0.7) + (afstand_in_cm * 0.3);*/
    return afstand_in_cm;
}




float ultrasoon_getDistance_R() {
    /*static float last_valid_distance = 30.0;
    static int zero_count = 0;*/

    static float afstand_in_cm = 30;

    if(nieuwe_meting_R){//alleen berekenen als de meetwaarde veranderd is
        nieuwe_meting_R = 0;
        unsigned long icrVal = ICR4;  // Use long to prevent overflow
        if(icrVal>550){
            unsigned long pulseTime_us = (((icrVal - 550) * PRESCALAR) / 16);
            afstand_in_cm = pulseTime_us / 58.0;
        }


        if (afstand_in_cm > max_stable_distance) {  // Fix stability issue at long distances
            afstand_in_cm = max_stable_distance;
        }
    }

    /*if (afstand_in_cm == 0) {
        zero_count++;
        if (zero_count >= 3) {
            return 0;  // Confirmed "wall lost"
        }
        return last_valid_distance;
    } else {
        zero_count = 0;
    }

    last_valid_distance = (last_valid_distance * 0.7) + (afstand_in_cm * 0.3);*/
    return afstand_in_cm;
}

