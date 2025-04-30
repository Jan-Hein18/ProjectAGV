
/*
 */

#include "Display1.h"
#include "StepperMotor.h"
#include "Ultrasoon.h"
#include "Knoppen.h"
#include "IRSensor.h"
#include "noodstop.h"
#include "clock.h"
#include "Navigatie.h"

#define turn_deadzone 0.05
#define stopTime 1
#define stopDetectTime 0.1

enum e_operatingState{eStop, startup,idle,module_1, module_2};
typedef enum e_operatingState t_operatingState;
t_operatingState operatingState = startup;

int main_new(void){
    //--INITIALISATIE--
    //systeem
    initClock();

    //navigatie
    ultrasoon_setup();
    stepperMotor_init();

    //tellen
    initSensoren();

    //ui
    _7segment_setup();
    initKnop();

    t_operatingState lastOperatingState = operatingState;
    t_operatingState previousOperatingSate = operatingState;
    while(1) {
        //buffer the previous operatingState when operatingState is changed
        if(lastOperatingState!=operatingState){
            previousOperatingSate = lastOperatingState;
        }
        lastOperatingState = operatingState;


        switch(operatingState){
        case eStop:{
            stopAGV();
            display_string("STOP");
            break;
        }
        case startup:{

            break;
        }
        case idle:{

            break;
        }
        case module_1:{

            break;
        }
        case module_2:{

            break;
        }
        }
    }

    return 0;
}

