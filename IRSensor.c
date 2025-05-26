#include "IRSensor.h"

#include <avr/io.h>

#include "clock.h"


#define IRREGISTER DDRK
#define IRPINS PINK
#define IRSENSOR PK0


#define DEBOUNCETIME_MS 50
#define DEBOUNCETIME_S (DEBOUNCETIME_MS*0.001)


void initSensoren(void) {
    IRREGISTER &= ~(1 << IRSENSOR);
}


int IRSensor_links(){
    static int IRSensorActief = 0;
    static float ActivatieTijd = 0;
    if(ActivatieTijd>time){
        ActivatieTijd = 0;
    }

    if((!(IRPINS&(1<<IRSENSOR)))&&(!IRSensorActief)&&((ActivatieTijd+DEBOUNCETIME_S)<time)){
        IRSensorActief = 1;
        ActivatieTijd = time;
    }
    else if((IRPINS&(1<<IRSENSOR))&&(IRSensorActief)&&((ActivatieTijd+DEBOUNCETIME_S)<time)){
        IRSensorActief = 0;
        ActivatieTijd = time;
    }

    return IRSensorActief;
}

