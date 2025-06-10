#include "noodstop.h"

#include "util/delay.h"
#include "avr/io.h"
#include "avr/interrupt.h"

#define noodstop_DDR    DDRB
#define noodstop_PORT   PORTB
#define noodstop_PIN    PINB
#define noodstop_BIT    (1<<PB2)

#include "Display1.h"

//access to operatingstate in main
enum enum_operatingState{e_eStop, e_reset, e_idle, e_pad, e_bocht, e_end};
typedef enum enum_operatingState t_operatingState;
extern t_operatingState operatingState;

void noodstop_Setup(){
    noodstop_DDR &= ~noodstop_BIT;
    noodstop_PORT |= noodstop_BIT;
    EICRA |= ISC20|ISC21;
    EIMSK |= INT2;
    sei();
}

int noodstop_Actief(){
    return !(noodstop_PIN&noodstop_BIT);
}

ISR(INT2_vect){
    //run once when e-stop active
    operatingState = e_eStop;

    _delay_ms(20);//account for bouncing before leaving interrupt
    /*while(noodstop_PIN&noodstop_BIT){//run continuous while e-stop active
        display_string("STOP");
    }*/


    //run once when e-stop deactivated

}

