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

enum enum_richting{e_links = 0x01,e_rechts = 0x02, e_vooruit, e_achteruit, e_blockblockL, e_blockblockR};
typedef enum enum_richting t_richting;

#define ROUTELENGTE 4
const t_richting route[ROUTELENGTE] = {e_vooruit, e_rechts, e_vooruit, e_links};

enum enum_operatingState{e_eStop, e_reset, e_idle, e_route, e_end};
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
            else if(knop_ingedrukt(e_plusKnop)||knop_ingedrukt(e_minKnop)){
                continueOperation = !continueOperation;
            }
            break;
        }
        case e_reset:{//reset waardes waar nodig en runt setup code
            if((lastOperatingState!=e_reset)){//dit gebeurt 1 keer
                //--INITIALISATIE--
                //systeem
                initClock();

                //navigatie
                ultrasoon_setup();

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
            if((!knop_ingedrukt(e_startKnop))&&(!knop_ingedrukt(e_plusKnop))&&(!knop_ingedrukt(e_minKnop))){
                operatingState = e_idle;
            }

            break;
        }
        case e_idle:{
            //stopAGV();

            display_string("idle");

            if(knop_ingedrukt(e_startKnop)){
                operatingState = e_route;
            }
            break;
        }
        case e_route:{
            //manage the current part of the route
            static int currentSection = 0;
            static t_richting currentDir = route[0];
            static int nextSection = 1;
            static int newSection = 0;
            newSection = 0;
            if(nextSection){
                currentSection++;
                if((currentSection++)>=ROUTELENGTE){//pad klaar
                    operatingState = e_end;
                    currentSection = 0;
                    nextSection = 1;
                    break;
                }
                currentDir = route[currentSection];
                newSection = 1;
            }


            switch(currentDir){
            case e_vooruit:{
                if(newSection){
                    while(!com_rechtCommand(0xff,0xff,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_achteruit:{
                if(newSection){
                    while(!com_rechtCommand(0x00,0xff,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_links:{
                if(newSection){
                    while(!com_bochtCommand(e_links,0xff,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_rechts:{
                if(newSection){
                    while(!com_bochtCommand(e_rechts,0xff,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_blockblockL:{
                if(newSection){
                    while(!com_blokBlokCommand(e_links,0xff,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_blockblockR:{
                if(newSection){
                    while(!com_blokBlokCommand(e_rechts,0xff,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
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

