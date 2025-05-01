#include "Display1.h"
#include "StepperMotor.h"
#include "Ultrasoon.h"
#include "Knoppen.h"
#include "IRSensor.h"
#include "noodstop.h"
#include "clock.h"
#include "Navigatie.h"

#define MAXWALLDISTANCE 15

#define PADAFSTAND 35
#define BOCHTAANTAL 3
t_richting bochten[BOCHTAANTAL] = {e_rechts, e_links, e_rechts};

enum enum_operatingState{e_eStop, e_reset, e_idle, e_pad, e_bocht};
typedef enum enum_operatingState t_operatingState;
t_operatingState operatingState = e_reset;

int main_new(void){
    t_operatingState lastOperatingState = operatingState; //operating state in last cycle
    t_operatingState previousOperatingSate = operatingState; //operating state before last change
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
        case e_reset:{
            //--INITIALISATIE--
            //systeem
            initClock();

            //navigatie
            ultrasoon_setup();
            stepperMotor_init();
            navigatie_setup();
            navigatie_setSpeed(3);
            navigatie_setAcceleratie(3);

            //tellen
            initSensoren();

            //ui
            _7segment_setup();
            knop_setup();

            //--SET NEUTRAL STATE--
            stopAGV();
            _7segment_write(0,0);

            operatingState = e_idle;
            break;
        }
        case e_idle:{
            stopAGV();

            if(knop_ingedrukt(e_startKnop)){
                operatingState = e_pad;
            }
            break;
        }
        case e_pad:{
            if((ultrasoon_getDistance_L()>MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()>MAXWALLDISTANCE)){
                    operatingState = e_bocht;
            }
            break;
        }
        case e_bocht:{
            static int bochtNr = 0;
            if((lastOperatingState==e_pad)){
                bochtNr++;
                if(bochtNr>=BOCHTAANTAL){
                    bochtNr = 0;
                    operatingState = e_reset;
                    break;
                }
            }

            navigatie_navigeerBocht(bochten[bochtNr],PADAFSTAND/2);

            break;
        }
        }
    }

    return 0;
}

