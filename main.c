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

int begin = 0;
int pakketten = 0;



int main(void){
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
    knop_setup();

    stopAGV();



    while(1) {
        //navigatie
        static int rijden = 1;
        if(rijden){
            navigeer_pad();
        }
        else{
            stopAGV();
        }


        //tellen
        static int rijdstop0 = 0;
        static float stopUntill0 = 0;
        static float stopDetectUntill0 = 0;
        static int pakket_gedetecteerd0 = 0;

        //links??
        if(detecteerPakket0()&&!pakket_gedetecteerd0&&time>stopDetectUntill0){//detecteer pakket
            pakket_gedetecteerd0 = 1;
            pakketten++;
            teller = pakketten;
            stopUntill0 = time+stopTime;
            rijdstop0 = 1;
        }
        else if(time>stopUntill0&&time>stopDetectUntill0&&pakket_gedetecteerd0){//ga door na 1 seconde
            rijdstop0 = 0;
            stopDetectUntill0 = time+stopDetectTime;
            if(!detecteerPakket0()){
                pakket_gedetecteerd0 = 0;
            }
        }
        //rechts??
        static int rijdstop1 = 0;
        static float stopUntill1 = 0;
        static float stopDetectUntill1 = 0;
        static int pakket_gedetecteerd1 = 0;
        if(detecteerPakket1()&&!pakket_gedetecteerd1&&time>stopDetectUntill1){//detecteer pakket
            pakket_gedetecteerd1 = 1;
            pakketten++;
            teller = pakketten;
            stopUntill1 = time+stopTime;
            rijdstop1 = 1;
        }
        else if(time>stopUntill1&&time>stopDetectUntill1&&pakket_gedetecteerd1){//ga door na 1 seconde
            rijdstop1 = 0;
            stopDetectUntill1 = time+stopDetectTime;
            if(!detecteerPakket1()){
                pakket_gedetecteerd1 = 0;
            }
        }

        if(rijdstop0||rijdstop1){
            rijden = 0;
        }
        else{
            rijden = 1;
        }

        if(pakketten>=verwachtAantalPakketten){
            rijden = 0;
            if((int)(time*2)%2){
            displayAantalPakketten();
            }
            else{_7segment_write(0,0);}
        }
        else{
            displayAantalPakketten();
        }


    }
    return 0;
}

