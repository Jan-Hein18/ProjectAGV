#include "Knoppen.h"

#include <avr/io.h>

#include "clock.h"


#define KNOPREGISTER    DDRF
#define KNOPPINS        PINF
#define KNOPPORTS       PORTF

#define STARTKNOP   PF0

#define DEBOUNCETIME_MS 10
#define DEBOUNCETIME_S (DEBOUNCETIME_MS*0.001)



void knop_setup(){
    KNOPREGISTER |= (1 << STARTKNOP);
    KNOPPORTS |= (1 << STARTKNOP);
}



//return knopstaat en houdt rekening met debouncing
int knop_ingedrukt(t_knoppen knop){
    switch(knop){
        case e_startKnop:{
            static int knopIngedrukt = 0;
            static float knopTijd = 0;
            if(knopTijd>time){
                knopTijd = 0;
            }

            if((!(KNOPPINS&(1<<STARTKNOP)))&&(!knopIngedrukt)&&((knopTijd+DEBOUNCETIME_S)<time)){
                knopIngedrukt = 1;
                knopTijd = time;
            }
            else if((KNOPPINS&(1<<STARTKNOP))&&(knopIngedrukt)&&((knopTijd+DEBOUNCETIME_S)<time)){
                knopIngedrukt = 0;
                knopTijd = time;
            }

            return knopIngedrukt;
        }

    }
    return 0;
}
