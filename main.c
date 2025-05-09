#include "Display1.h"
#include "StepperMotor.h"
#include "Ultrasoon.h"
#include "Knoppen.h"
#include "IRSensor.h"
#include "noodstop.h"
#include "clock.h"
#include "Navigatie.h"
#include "pakketten.h"
#include "com_agv.h"


#define PAKKETAANTAL 5

#define MAXWALLDISTANCE 15

#define SPEED 0.125

#define PADAFSTAND 36
#define BOCHTAANTAL 1
t_richting bochten[BOCHTAANTAL] = {e_rechts};//, e_links, e_rechts};

enum enum_operatingState{e_eStop, e_reset, e_idle, e_pad, e_bochtState, e_blockBlock, e_volg};
typedef enum enum_operatingState t_operatingState;
t_operatingState operatingState = e_reset;

int main(void){
    t_operatingState lastOperatingState = operatingState; //operating state in last cycle
    t_operatingState previousOperatingSate = operatingState; //operating state before last change
    t_operatingState startOperatingState = operatingState;
    while(1) {
        //buffer the previous operatingState when operatingState is changed
        startOperatingState = operatingState;
        if(lastOperatingState!=operatingState){
            previousOperatingSate = lastOperatingState;
        }


        switch(startOperatingState){
        case e_eStop:{
            stopAGV();
            break;
        }
        case e_reset:{
            static int reset = 0;
            if(!reset){
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
                aantalPakketten = 0;

                //ui
                _7segment_setup();
                knop_setup();

                //com
                com_setup();

                //--SET NEUTRAL STATE--
                stopAGV();
                _7segment_write(0,0);

                reset = 1;
            }


            display_string("rset");

            //geen knoppen ingedrukt
            if((!knop_ingedrukt(e_startKnop))&&(!knop_ingedrukt(e_plusKnop))&&(!knop_ingedrukt(e_minKnop))){
                reset = 0;
                operatingState = e_idle;
            }

            break;
        }
        case e_idle:{
            stopAGV();
            break;
        }
        case e_pad:{
            navigatie_navigeerPad();


            if((ultrasoon_getDistance_L()>MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()>MAXWALLDISTANCE)){
                com_doneCommand();
                operatingState = e_idle;
            }
            break;
        }
        case e_bochtState:{
            static float startAfstand = 0;
            switch(lastOperatingState){
            case e_bochtState:
            case e_idle:
            case e_eStop:{
                break;
            }
            default:{
                startAfstand = navigatie_afstandAfgelegd;
                break;
            }
            };
            navigatie_navigeerBocht(com_command.arg,PADAFSTAND/2);
            if((navigatie_afstandAfgelegd-startAfstand)>(3.14*(PADAFSTAND*0.01)/2)){
                com_doneCommand();
                operatingState = e_idle;
            }

            break;
        }
        case e_blockBlock:{
            stopAGV();
            break;
        }
        case e_volg:{
            stopAGV();
            break;
        }
        }
        lastOperatingState = startOperatingState;
    }

    return 0;
}

