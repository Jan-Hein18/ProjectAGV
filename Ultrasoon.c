#include "Ultrasoon.h"
#include "avr/io.h"
#include "avr/interrupt.h"
#include "clock.h"

#define ULTRASOON_L_TRIG1_BIT PL3  // OC5A
#define ULTRASOON_L_TRIG1_DDR DDRL
#define ULTRASOON_L_TRIG2_BIT PL4  // OC5B
#define ULTRASOON_L_TRIG2_DDR DDRL
#define ULTRASOON_L_TRIG3_BIT PL5  // OC5C
#define ULTRASOON_L_TRIG3_DDR DDRL
#define ULTRASOON_L_ECHO_BIT PL1  // ICP5
#define ULTRASOON_L_ECHO_DDR DDRL

#define ULTRASOON_R_TRIG1_BIT PH3  // OC4A
#define ULTRASOON_R_TRIG1_DDR DDRH
#define ULTRASOON_R_TRIG2_BIT PH4  // OC4B
#define ULTRASOON_R_TRIG2_DDR DDRH
#define ULTRASOON_R_TRIG3_BIT PH5  // OC4C
#define ULTRASOON_R_TRIG3_DDR DDRH
#define ULTRASOON_R_ECHO_BIT PL0  // ICP4
#define ULTRASOON_R_ECHO_DDR DDRL

#define PRESCALAR 64

#define ULTRASOON_MAXDISTANCE 20
#define ULTRASOON_MINDISTANCE 1

#define MAXWALLDISTANCE 13
#define DEBOUNCETIME_MS 75
#define DEBOUNCETIME_S (DEBOUNCETIME_MS*0.001)


volatile uint8_t nieuwe_meting_L = 0;
volatile uint8_t nieuwe_meting_R = 0;
volatile unsigned long ICR5temp = 0;
volatile unsigned long ICR4temp = 0;



// ISR voor linker sensor (echo ontvangen)
ISR(TIMER5_CAPT_vect) {
    //start nieuwe meting
    TCNT5 = 0;
    ICR5temp = ICR5;

    nieuwe_meting_L = 1;
}

// ISR voor rechter sensor (echo ontvangen)
ISR(TIMER4_CAPT_vect) {
    //start nieuwe meting
    TCNT4 = 0;
    ICR4temp = ICR4;

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

    ULTRASOON_L_TRIG1_DDR |= (1 << ULTRASOON_L_TRIG1_BIT);  // Trigger als output
    ULTRASOON_L_TRIG2_DDR |= (1 << ULTRASOON_L_TRIG2_BIT);  // Trigger als output
    ULTRASOON_L_TRIG3_DDR |= (1 << ULTRASOON_L_TRIG3_BIT);  // Trigger als output
    ULTRASOON_L_ECHO_DDR &= ~(1 << ULTRASOON_L_ECHO_BIT); // Echo als input

    // Timer 5 instellen voor 10 bit PWM en input capture
    TCCR5A |= (1 << WGM50) | (1 << WGM51);
    TCCR5B |= (1 << WGM52) | (0 << WGM53);
    TCCR5A |= (1 << COM5B1);
    TCCR5A |= (1 << COM5A1) | (1 << COM5C1);
    TCCR5B |= (1 << CS50) | (1 << CS51);

    //zet output uit
    OCR5A = 0x03FF;      // 12us puls = 3
    OCR5B = 0x03FF;      // 12us puls = 3
    OCR5C = 0x03FF;      // 12us puls = 3


    // Input capture instellen
    TCCR5B &= ~(1 << ICES5); // Start op vallende flank
    TIMSK5 |= (1 << ICIE5);  // Interrupt inschakelen
}

void ultrasoon_setup_R() {
    TCCR4B &= ~((1 << CS40) | (1 << CS41));//stop clock
    ICR4 = 0;
    nieuwe_meting_R = 0;

    ULTRASOON_R_TRIG1_DDR |= (1 << ULTRASOON_R_TRIG1_BIT);
    ULTRASOON_R_TRIG2_DDR |= (1 << ULTRASOON_R_TRIG2_BIT);
    ULTRASOON_R_TRIG3_DDR |= (1 << ULTRASOON_R_TRIG3_BIT);
    ULTRASOON_R_ECHO_DDR &= ~(1 << ULTRASOON_R_ECHO_BIT);

    //10 bit pwm
    TCCR4A |= (1 << WGM40) | (1 << WGM41);
    TCCR4B |= (1 << WGM42) | (0 << WGM43);
    TCCR4A |= (1 << COM4B1);
    TCCR4A |= (1 << COM4A1) | (1 << COM4C1);
    TCCR4B |= (1 << CS40) | (1 << CS41);

    OCR4A = 0x03FF;
    OCR4B = 0x03FF;
    OCR4C = 0x03FF;


    TCCR4B &= ~(1 << ICES4);
    TIMSK4 |= (1 << ICIE4);
}


void ultrasoon_switchSensor(t_ultrasoon_sensorstype sensor){
    switch(sensor){ //12us pulse
    case e_ultrasooonAchter:{
        OCR4A = 3;
        OCR5A = 3;


        OCR4B = 0x03FF;
        OCR4C = 0x03FF;

        OCR5B = 0x03FF;
        OCR5C = 0x03FF;
        break;
    }
    case e_ultrasooonVoor:{
        OCR4B = 3;
        OCR5B = 3;

        OCR4A = 0x03FF;
        OCR4C = 0x03FF;

        OCR5A = 0x03FF;
        OCR5C = 0x03FF;
        break;
    }
    case e_ultrasooonVolg:{
        OCR4C = 3;
        OCR5C = 3;

        OCR4B = 0x03FF;
        OCR4A = 0x03FF;

        OCR5B = 0x03FF;
        OCR5A = 0x03FF;
        break;
    }
    }
}


float ultrasoon_getDistance_L() {
    static float afstand_in_cm = 0;
    static float old = 0;
    static int OOR = 0; //out of range

    if(nieuwe_meting_L){//alleen berekenen als de meetwaarde veranderd is
        nieuwe_meting_L = 0;
        unsigned long icrVal = ICR5temp;  // Use long to prevent overflow
        if(icrVal>550){
            unsigned long pulseTime_us = (((icrVal - 550) * (unsigned long long)PRESCALAR) / 16);
            afstand_in_cm = pulseTime_us / 58.0;
        }
        else{
            afstand_in_cm = ULTRASOON_MAXDISTANCE;
        }

        if (afstand_in_cm >= ULTRASOON_MAXDISTANCE) {  // Fix stability issue at long distances
            afstand_in_cm = ULTRASOON_MAXDISTANCE;
            if(!OOR){
                afstand_in_cm = old;
            }
            OOR = 1;
        }
        else if(afstand_in_cm<ULTRASOON_MINDISTANCE){
            afstand_in_cm = ULTRASOON_MINDISTANCE;
            OOR = 0;
        }
        else{
            OOR = 0;
        }
    }
    old = afstand_in_cm;
    return afstand_in_cm;
}




float ultrasoon_getDistance_R() {
    static float afstand_in_cm = 0;
    static float old = 0;
    static int OOR = 0; //out of range

    if(nieuwe_meting_R){//alleen berekenen als de meetwaarde veranderd is
        nieuwe_meting_R = 0;
        unsigned long icrVal = ICR4temp;  // Use long to prevent overflow
        if(icrVal>550){
            unsigned long pulseTime_us = (((icrVal - 550) * (unsigned long long)PRESCALAR) / 16);
            afstand_in_cm = pulseTime_us / 58.0;
        }
        else{afstand_in_cm = ULTRASOON_MAXDISTANCE;}


        if (afstand_in_cm >= ULTRASOON_MAXDISTANCE) {  // Fix stability issue at long distances
            afstand_in_cm = ULTRASOON_MAXDISTANCE;
            if(!OOR){
                afstand_in_cm = old;
            }
            OOR = 1;
        }
        else if(afstand_in_cm<ULTRASOON_MINDISTANCE){
            afstand_in_cm = ULTRASOON_MINDISTANCE;
            OOR = 0;
        }
        else{
            OOR = 0;
        }
    }
    old = afstand_in_cm;
    return afstand_in_cm;
}


int ultrasoon_wallGoneL(){
    static double lastCalled = 0;
    static int gone = 0;
    static double lastUnchanged = 0;


    if((time-lastCalled)>(DEBOUNCETIME_S)){
            lastUnchanged = time;
    }
    lastCalled = time;


    if(1){//!gone){
        if(ultrasoon_getDistance_L()>MAXWALLDISTANCE){
            if((time-lastUnchanged)>DEBOUNCETIME_S){
                gone = 1;
            }
        }
        else{
            lastUnchanged = time;
            gone = 0;
        }
    }
    else{
        if(!(ultrasoon_getDistance_L()>MAXWALLDISTANCE)){
            if((time-lastUnchanged)>DEBOUNCETIME_S){
                gone = 0;
            }
        }
        else{
            lastUnchanged = time;
            gone = 1;
        }
    }
    return gone;
}

int ultrasoon_wallGoneR(){
    static double lastCalled = 0;
    static int gone = 0;
    static double lastUnchanged = 0;


    if((time-lastCalled)>(DEBOUNCETIME_S)){
            lastUnchanged = time;
    }
    lastCalled = time;


    if(1){//!gone){
        if(ultrasoon_getDistance_R()>MAXWALLDISTANCE){
            if((time-lastUnchanged)>DEBOUNCETIME_S){
                gone = 1;
            }
        }
        else{
            lastUnchanged = time;
            gone = 0;
        }
    }
    else{
        if(!(ultrasoon_getDistance_R()>MAXWALLDISTANCE)){
            if((time-lastUnchanged)>DEBOUNCETIME_S){
                gone = 0;
            }
        }
        else{
            lastUnchanged = time;
            gone = 1;
        }
    }
    return gone;
}
