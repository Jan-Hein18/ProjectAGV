#include "metaalDetector.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"


#define METAALDETECTOR_L_TRIG_DDR   DDRH
#define METAALDETECTOR_L_TRIG_BIT   PH4
#define METAALDETECTOR_L_ECHO_DDR   DDRL
#define METAALDETECTOR_L_ECHO_BIT   PL0

#define METAALDETECTOR_R_TRIG_DDR   DDRL
#define METAALDETECTOR_R_TRIG_BIT   PL4
#define METAALDETECTOR_R_ECHO_DDR   DDRL
#define METAALDETECTOR_R_ECHO_BIT   PL1



#define DETECTCYCLE 2400 //60us
#define DETECTMARGION 10
#define PWMDUTYCYCLE 64000 //1 ms
#define PWMPERIOD 2000 //250us
#define SIGNALFALLPERIOD 960 // 60us
#define DEBOUNCETIME_MS 10
#define DEBOUNCETIME_S (DEBOUNCETIME_MS*0.001)


//metaal links
volatile int metaalLinks = 0;
volatile long int ICRL = 0;

volatile long int ICRBUFFL[100];
volatile int bufflocL = 0;

volatile unsigned long int setPointL = 0;
volatile unsigned long int setPointR = 0;


ISR(TIMER4_CAPT_vect){
    long int ICRval = ICR4;
    if((ICRval>SIGNALFALLPERIOD)&&(ICRval<PWMPERIOD*0.95)){

        ICRBUFFL[bufflocL] = ICRval;
        bufflocL++;
        if(bufflocL>=100){
            bufflocL = 0;
        }

        unsigned long int temp1 = 0;

        for(int i = 0; i<100; i++){
            temp1+=ICRBUFFL[i];
        }
        temp1/=100;


        ICRL = temp1;


        metaalLinks = ICRL>(setPointL+DETECTMARGION);
    }
}


//metaal rechts
volatile int metaalRechts = 0;
volatile long int ICRR = 0;

volatile long int ICRBUFFR[100];
volatile int bufflocR = 0;

ISR(TIMER5_CAPT_vect){
    long int ICRval = ICR5;
    if((ICRval>SIGNALFALLPERIOD)&&(ICRval<PWMPERIOD*0.95)){

        ICRBUFFR[bufflocR] = ICRval;
        bufflocR++;
        if(bufflocR>=100){
            bufflocR = 0;
        }

        unsigned long int temp1 = 0;

        for(int i = 0; i<100; i++){
            temp1+=ICRBUFFR[i];
        }
        temp1/=100;


        ICRR = temp1;//ICRval;//temp1>temp2;


        metaalRechts = ICRR>(setPointR+DETECTMARGION);
    }
}


void metaaldetector_setup_L() {
    TCCR4B &= ~((1 << CS40) | (1 << CS41));//stop clock
    ICR4 = 0;

    METAALDETECTOR_L_TRIG_DDR |= (1 << METAALDETECTOR_L_TRIG_BIT);  // Trigger als output
    METAALDETECTOR_L_ECHO_DDR &= ~(1 << METAALDETECTOR_L_ECHO_BIT); // Echo als input

    // Timer 5 instellen voor PWM max freq
    TCCR4A |= (1 << WGM40) | (1 << WGM41);
    TCCR4B |= (1 << WGM42) | (1 << WGM43);
    TCCR4A |= (1 << COM4B1);
    TCCR4B |= (1 << CS40);

    OCR4A = PWMDUTYCYCLE;
    OCR4B = PWMPERIOD;

    // Input capture instellen
    TCCR4B &= ~(1 << ICES4); // Start op vallende flank
    TIMSK4 |= (1 << ICIE4);  // Interrupt inschakelen
}

void metaaldetector_setup_R() {
    TCCR5B &= ~((1 << CS50) | (1 << CS51));//stop clock
    ICR5 = 0;

    METAALDETECTOR_R_TRIG_DDR |= (1 << METAALDETECTOR_R_TRIG_BIT);  // Trigger als output
    METAALDETECTOR_R_ECHO_DDR &= ~(1 << METAALDETECTOR_R_ECHO_BIT); // Echo als input

    // Timer 5 instellen voor PWM max freq
    TCCR5A |= (1 << WGM50) | (1 << WGM51);
    TCCR5B |= (1 << WGM52) | (1 << WGM53);
    TCCR5A |= (1 << COM5B1);
    TCCR5B |= (1 << CS50);

    OCR5A = PWMDUTYCYCLE;
    OCR5B = PWMPERIOD;

    // Input capture instellen
    TCCR5B &= ~(1 << ICES5); // Start op rijzende flank
    TIMSK5 |= (1 << ICIE5);  // Interrupt inschakelen
}

void metaaldetector_setup() {
    metaaldetector_setup_L();

    metaaldetector_setup_R();
    sei();  // Schakel interrupts in
}


int metaaldetector_links(){
    return metaalLinks;
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
    return metaalRechts;
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


void metaaldetector_setPointL(){
    setPointL = ICRL;
}

void metaaldetector_setPointR(){
    setPointR = ICRR;
}
