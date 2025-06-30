#include "IRSensor.h"

#include <avr/io.h>

#include "clock.h"


#define IRREGISTER DDRH
#define IRPINS PINH
#define IRSENSOR_L PH5
#define IRSENSOR_R PH6

#define DEBOUNCETIME_MS 50
#define DEBOUNCETIME_S (DEBOUNCETIME_MS*0.001)


void initSensoren(void) {
    IRREGISTER &= ~((1 << IRSENSOR_L) | (1 << IRSENSOR_R));
}


int IRSensor_links(){
    static int IRSensorActief = 0;
    static float ActivatieTijd = 0;
    if(ActivatieTijd>time){
        ActivatieTijd = 0;
    }

    if((!(IRPINS&(1<<IRSENSOR_L)))&&(!IRSensorActief)&&((ActivatieTijd+DEBOUNCETIME_S)<time)){
        IRSensorActief = 1;
        ActivatieTijd = time;
    }
    else if((IRPINS&(1<<IRSENSOR_L))&&(IRSensorActief)&&((ActivatieTijd+DEBOUNCETIME_S)<time)){
        IRSensorActief = 0;
        ActivatieTijd = time;
    }

    return IRSensorActief;
}

int IRSensor_rechts(){
    static int IRSensorActief = 0;
    static float ActivatieTijd = 0;
    if(ActivatieTijd>time){
        ActivatieTijd = 0;
    }

    if((!(IRPINS&(1<<IRSENSOR_R)))&&(!IRSensorActief)&&((ActivatieTijd+DEBOUNCETIME_S)<time)){
        IRSensorActief = 1;
        ActivatieTijd = time;
    }
    else if((IRPINS&(1<<IRSENSOR_R))&&(IRSensorActief)&&((ActivatieTijd+DEBOUNCETIME_S)<time)){
        IRSensorActief = 0;
        ActivatieTijd = time;
    }

    return IRSensorActief;
}
