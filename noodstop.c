#include "noodstop.h"

#include "util/delay.h"
#include "avr/io.h"
#include "avr/interrupt.h"

#define noodstop_DDR    DDRB
#define noodstop_PORT   PORTB
#define noodstop_PIN    PINB
#define noodstop_BIT    (1<<PB2)

#include "Display1.h"


void noodstop_Setup(){
    noodstop_DDR &= ~noodstop_BIT;
    noodstop_PORT |= noodstop_BIT;
    EICRA |= ISC20|ISC21;
    EIMSK |= INT2;
    sei();
}


ISR(INT2_vect){
    //run once when e-stop active



    _delay_ms(20);
    if(noodstop_PIN&noodstop_BIT){//run continuous while e-stop active
        display(1000);
    }


    //run once when e-stop deactivated


}

