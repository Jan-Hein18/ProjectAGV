#include "Display1.h"
#include "Ultrasoon.h"
#include "Knoppen.h"
#include "IRSensor.h"
#include "noodstop.h"
#include "clock.h"
#include "pakketten.h"
#include "com_module.h"


#define MAXWALLDISTANCE 15

#define PADAFSTAND 36
#define ROUTELENGTE 1

enum enum_richting{e_links = 0x01,e_rechts = 0x02, e_vooruit, e_achteruit, e_blockblock};
typedef enum enum_richting t_richting;

t_richting route[ROUTELENGTE] = {e_vooruit, };//, e_links, e_rechts};

enum enum_operatingState{e_eStop, e_reset, e_idle, e_pad, e_bocht, e_end};
typedef enum enum_operatingState t_operatingState;
t_operatingState operatingState = e_reset;

int main(void){
    t_operatingState lastOperatingState = -1; //operating state in last cycle
    t_operatingState previousOperatingSate = -1; //operating state before last change
    t_operatingState currentOperatingState = -1; //operating state at start of cycle, do not change during cycle

    while(1) {
        //buffer the previous operatingState when operatingState is changed
        currentOperatingState = operatingState;
        if(lastOperatingState!=operatingState){
            previousOperatingSate = lastOperatingState;
        }


        switch(currentOperatingState){
        case e_eStop:{
            static int continueOperation = 0;

            //stopAGV();

            if(((int)time*10)%5){
                display_string(continueOperation?"cont":"rset");
            }
            else{
                display_string("STOP");
            }


            if(knop_ingedrukt(e_startKnop)){
                operatingState = continueOperation?previousOperatingSate:e_reset;
            }


            break;
        }
        case e_reset:{//reset waardes waar nodig en runt setup code
            if((lastOperatingState!=e_reset)){//dit gebeurt 1 keer
                //--INITIALISATIE--
                //systeem
                initClock();


                //tellen
                initSensoren();
                aantalPakketten = 0;

                //ui
                _7segment_setup();
                knop_setup();

                //mi
                com_setup();



                //--SET NEUTRAL STATE--
                //stopAGV();
                _7segment_write(0,0);
            }


            display_string("rset");

            //wacht tot geen knoppen ingedrukt
            if((!knop_ingedrukt(e_startKnop))){
                operatingState = e_idle;
            }

            break;
        }
        case e_idle:{
            //stopAGV();

            display_string("idle");

            if(knop_ingedrukt(e_startKnop)){
                operatingState = e_pad;
            }
            break;
        }
        case e_pad:{
            //wat te doen in pad



            //eindconditie om over te gaan naar bocht

            break;
        }
        case e_bocht:{
            static int bochtNr = 0;
            if((lastOperatingState==e_pad)){//runt een keer wanneer status veranderd naar bocht vanuit pad
                bochtNr++;

                if(bochtNr>BOCHTAANTAL){
                    bochtNr = 0;
                    operatingState = e_end;//stop na laatste bocht
                    break;
                }
            }



            if(com_agvDone){//klaar met bocht
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
        lastOperatingState = currentOperatingState;
    }

    return 0;
}

