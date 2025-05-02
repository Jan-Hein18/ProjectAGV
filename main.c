#include "Display1.h"
#include "StepperMotor.h"
#include "Ultrasoon.h"
#include "Knoppen.h"
#include "IRSensor.h"
#include "noodstop.h"
#include "clock.h"
#include "Navigatie.h"
#include "pakketten.h"


#define PAKKETAANTAL 5

#define MAXWALLDISTANCE 15

#define PADAFSTAND 35
#define BOCHTAANTAL 3
t_richting bochten[BOCHTAANTAL] = {e_rechts, e_links, e_rechts};

enum enum_operatingState{e_eStop, e_reset, e_idle, e_pad, e_bocht, e_end};
typedef enum enum_operatingState t_operatingState;
t_operatingState operatingState = e_reset;

int main(void){
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
            static int continueOperation = 0;

            stopAGV();

            if(((int)time*10)%5){
                display_string(continueOperation?"cont":"rset");
            }
            else{
                display_string("STOP");
            }


            if(knop_ingedrukt(e_startKnop)){
                operatingState = continueOperation?previousOperatingSate:e_reset;
            }
            else if(knop_ingedrukt(e_plusKnop)||knop_ingedrukt(e_minKnop)){
                continueOperation = !continueOperation;
            }
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

            display_string("idle");

            if(knop_ingedrukt(e_startKnop)){
                operatingState = e_pad;
            }
            break;
        }
        case e_pad:{
            navigatie_navigeerPad();

            telPakketten();
            display_getal(PAKKETAANTAL*100+aantalPakketten);

            if(aantalPakketten>=PAKKETAANTAL){
                operatingState = e_end;
            }
            else if((ultrasoon_getDistance_L()>MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()>MAXWALLDISTANCE)){
                    operatingState = e_bocht;
            }
            break;
        }
        case e_bocht:{
            static int bochtNr = 0;
            static int startAfstand = 0;
            if((lastOperatingState==e_pad)){
                bochtNr++;
                startAfstand = navigatie_afstandAfgelegd;
                if(bochtNr>=BOCHTAANTAL){
                    bochtNr = 0;
                    operatingState = e_end;
                    break;
                }
            }

            navigatie_navigeerBocht(bochten[bochtNr],PADAFSTAND/2);

            if((navigatie_afstandAfgelegd-startAfstand)>(3.14*PADAFSTAND/2)){
                operatingState = e_pad;
            }
            break;
        }
        case e_end:{
            display_string("end ");

            if(knop_ingedrukt(e_startKnop)){
                operatingState = e_reset;
            }
        }
        }
    }

    return 0;
}

