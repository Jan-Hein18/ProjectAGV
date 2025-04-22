#include "IRSensor.h"
#include "util/delay.h"

int pakketGeteld = 0;
int aantalPakketten = 0;

void initSensoren(void) {
    //IRRegister |= (1 << IRSensor1) | (1 << IRSensor2);
    //IRPins |= (1 << IRSensor1) | (1 << IRSensor2);
}

int detecteerPakket0(void) {
    int detect1 = 0;
    if(!(IRPins & (1 << IRSensor1))) {
        detect1 =1;
    }

    if(detect1){
        _delay_ms(10);
        if(!(IRPins & (1 << IRSensor1))) {
            return 1;
        }
    }
    return 0;
}

int detecteerPakket1(void) {
    int detect2 = 0;
    if(!(IRPins &(1 << IRSensor2))) {
        detect2 =1;
    }

    if(detect2){
        _delay_ms(10);
        if(!(IRPins &(1 << IRSensor2))) {
        return 1;
        }
    }
    return 0;
}

int telPakketten(void) {
    if(detecteerPakket1()||detecteerPakket0()) {
        if(pakketGeteld == 0) {
            aantalPakketten++;
            pakketGeteld = 1;
        }
    }
    return aantalPakketten;
}
