#include "IRSensor.h"

#include <avr/io.h>

#include "clock.h"


#define IRREGISTER DDRE
#define IRPINS PINE
#define IRSENSOR PE4


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

