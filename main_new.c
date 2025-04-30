#include "Display1.h"
#include "StepperMotor.h"
#include "Ultrasoon.h"
#include "Knoppen.h"
#include "IRSensor.h"
#include "noodstop.h"
#include "clock.h"
#include "Navigatie.h"


enum enum_operatingState{e_eStop, e_startup, e_idle, e_module_1, e_module_2};
typedef enum enum_operatingState t_operatingState;
t_operatingState operatingState = startup;

int main_new(void){
    t_operatingState lastOperatingState = operatingState;
    t_operatingState previousOperatingSate = operatingState;
    while(1) {
        //buffer the previous operatingState when operatingState is changed
        if(lastOperatingState!=operatingState){
            previousOperatingSate = lastOperatingState;
        }
        lastOperatingState = operatingState;


        switch(operatingState){
        case e_eStop:{
            stopAGV();
            display_string("STOP");
            break;
        }
        case e_startup:{
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

            //--SET NEUTRAL STATE--
            stopAGV();
            _7segment_write(0,0);

            operatingState = e_idle;
            break;
        }
        case e_idle:{

            break;
        }
        case e_module_1:{

            break;
        }
        case e_module_2:{

            break;
        }
        }
    }

    return 0;
}

