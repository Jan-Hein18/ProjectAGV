#include "noodstop.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "clock.h"

#define DEBOUNCETIME_MS 20
#define DEBOUNCETIME_S (DEBOUNCETIME_MS*0.001)

#define noodstop_DDR    DDRD
#define noodstop_PORT   PORTD
#define noodstop_PIN    PIND
#define noodstop_BIT    (1<<PD0)

#include "Display1.h"

//access to operatingstate in main
enum enum_operatingState{e_eStop, e_reset, e_idle, e_pad, e_bocht, e_end};
typedef enum enum_operatingState t_operatingState;
extern t_operatingState operatingState;

void noodstop_Setup(){
    noodstop_DDR &= ~noodstop_BIT;
    noodstop_PORT |= noodstop_BIT;
    EICRA &= ~(ISC00|ISC01);
    EIMSK |= (1 << INT0);
    sei();
}

int noodstop_ingedrukt(){
    static int knopIngedrukt = 0;
    static float knopTijd = 0;
    if(knopTijd>time){
        knopTijd = 0;
    }

    if((!(noodstop_PIN&noodstop_BIT))&&(!knopIngedrukt)&&((knopTijd+DEBOUNCETIME_S)<time)){
        knopIngedrukt = 1;
        knopTijd = time;
    }
    else if((noodstop_PIN&noodstop_BIT)&&(knopIngedrukt)&&((knopTijd+DEBOUNCETIME_S)<time)){
        knopIngedrukt = 0;
        knopTijd = time;
    }

    return knopIngedrukt;
}


ISR(INT0_vect){
    stopAGV();
    operatingState = e_eStop;
    //while(!com_sendCommand(50));

}

