#include "metaalDetector.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"


#define DETECTCYCLE 400
#define DEBOUNCETIME_MS 10
#define DEBOUNCETIME_S (DEBOUNCETIME_MS*0.001)


//metaal links
volatile int metaalLinks = 0;
ISR(TIMER4_CAPT_vect){
    metaalLinks = ICR4>DETECTCYCLE;

}

//metaal rechts
volatile int metaalRechts = 0;
ISR(TIMER5_CAPT_vect){
    metaalRechts = ICR5>DETECTCYCLE;
}


void metaaldetector_setup() {
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
    TCCR4B &= ~((1 << CS40) | (1 << CS41));//stop clock
    ICR4 = 0;
    nieuwe_meting_R = 0;

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




int metaaldetector_links(){
    static int metaalGedetecteerd = 0;
    static float detectieTijd = 0;
    if(detectieTijd>time){
        detectieTijd = 0;
    }

    if((!metaalLinks)&&(!metaalGedetecteerd)&&((detectieTijd+DEBOUNCETIME_S)<time)){
        metaalGedetecteerd = 1;
        detectieTijd = time;
    }
    else if((metaalLinks)&&(metaalGedetecteerd)&&((detectieTijd+DEBOUNCETIME_S)<time)){
        metaalGedetecteerd = 0;
        detectieTijd = time;
    }

    return metaalGedetecteerd;
}

int metaaldetector_rechts(){
    static int metaalGedetecteerd = 0;
    static float detectieTijd = 0;
    if(detectieTijd>time){
        detectieTijd = 0;
    }

    if((!metaalRechts)&&(!metaalGedetecteerd)&&((detectieTijd+DEBOUNCETIME_S)<time)){
        metaalGedetecteerd = 1;
        detectieTijd = time;
    }
    else if((metaalRechts)&&(metaalGedetecteerd)&&((detectieTijd+DEBOUNCETIME_S)<time)){
        metaalGedetecteerd = 0;
        detectieTijd = time;
    }

    return metaalGedetecteerd;
}
