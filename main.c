#include "Display1.h"
#include "Knoppen.h"
#include "IRSensor.h"
#include "noodstop.h"
#include "clock.h"
#include "pakketten.h"
#include "com_module.h"
#include "metaalDetector.h"

#define MAXPAKKETTEN 15

#define DETECTCYCLERESETTIME 0.5 //min duration between end of detect cycle and a new cycle
#define STOPTIMEATPACKAGE 1 //how long to stop at a package
#define MAXWALLDISTANCE 15

#define DRIVESPEED 0.125 //max 2
#define DRIVESPEEDSCALED DRIVESPEED*0xff/2 //scaled for use in command

#define PADAFSTAND 36

enum enum_richting{e_links = 0x01,e_rechts = 0x02, e_vooruit, e_achteruit, e_blockblockL, e_blockblockR};
typedef enum enum_richting t_richting;

#define ROUTELENGTE 3
const t_richting route[ROUTELENGTE] = {e_vooruit, e_rechts, e_vooruit};

enum enum_operatingState{e_eStop, e_reset, e_idle, e_route, e_end};
typedef enum enum_operatingState t_operatingState;
t_operatingState operatingState = e_reset;

int main(void){
    t_operatingState lastOperatingState = -1; //operating state in last cycle
    t_operatingState previousOperatingSate = -1; //operating state before last change
    t_operatingState currentOperatingState = -1; //operating state at start of cycle, do not change during cycle

    float lastCycleDuration = 0;
    float cycleStartTime = time;

    int resetRoute = 1;

    while(1) {
        //calculate cycleTimes
        lastCycleDuration = time-cycleStartTime;
        cycleStartTime = time;

        //buffer the previous operatingState when operatingState is changed
        currentOperatingState = operatingState;
        if(lastOperatingState!=operatingState){
            previousOperatingSate = lastOperatingState;
        }


        switch(currentOperatingState){
        case e_eStop:{
            static int continueOperation = 0;

            while(!com_rechtCommand(0xff/2,0,0xff));//stop

            if(((int)time*10)%5){
                display_string(continueOperation?"cont":"rset");
            }
            else{
                display_string("STOP");
            }


            if(!noodstop_Actief()&&knop_ingedrukt(e_startKnop)){
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
                cycleStartTime = 0;

                //tellen
                initSensoren();
                metaaldetector_setup();
                aantalPakkettenMetaal = 0;
                aantalPakkettenLeeg = 0;

                //ui
                _7segment_setup();
                knop_setup();

                //mi
                com_setup();

                //reset operatingStates
                resetRoute = 1;


                //--SET NEUTRAL STATE--
                while(!com_rechtCommand(0xff/2,0,0xff));//stop
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
            if(lastOperatingState!=e_idle){
                while(!com_rechtCommand(0xff/2,0,0xff));//stop
            }

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
            static int nextSection = 0;
            static int newSection = 1;
            newSection = 0;

            if(resetRoute){
                currentSection = 0;
                currentDir = route[0];
                nextSection = 0;
                newSection = 1;
            }

            if(nextSection){
                currentSection++;
                if(currentSection>=ROUTELENGTE){//pad klaar
                    operatingState = e_end;
                    resetRoute = 1;
                    break;
                }
                currentDir = route[currentSection];
                newSection = 1;
                nextSection = 0;
            }


            switch(currentDir){
            case e_vooruit:{
                static float dirStartTime = 0;

                if(newSection){
                    dirStartTime = time;
                    while(!com_rechtCommand(0xff,DRIVESPEEDSCALED,0xff));
                }





                //stop na 1 seconde bij 2e rechte stuk (stuk 2 van route)
                switch(currentSection){
                case 2:{
                    const float driveTimeToStop = 1;
                    if((dirStartTime+driveTimeToStop)<time){
                        while(!com_rechtCommand(0xff/2,0,0xff));//stop
                        nextSection = 1;
                    }
                    break;
                }
                default:{
                    //tel pakketten
                    const float driveTimeToStop = 0.1;
                    static int detect_L = 0; //is a detection cycle active
                    static int detect_R = 0;
                    static float detectTimeL = 0;//time when a package was sensed
                    static float detectTimeR = 0;
                    static int stoppedL = 0;
                    static int stoppedR = 0;


                    if((!detect_L)&&IRSensor_links()){//start detect cycle
                            detectTimeL = time;
                            detect_L = 1;
                    }

                    if((!detect_R)&&IRSensor_rechts()){//start detect cycle
                            detectTimeR = time;
                            detect_R = 1;
                    }



                    if(detect_L){
                        if(stoppedR){//shift cycle start to compensate for stopping by other sensor
                            detectTimeL+=lastCycleDuration;
                        }
                        else{
                            if(time<(detectTimeL+driveTimeToStop)){
                                //do nothing
                            }
                            else if(time<(detectTimeL+driveTimeToStop+1)){//stop for 1 second and count
                                if(!stoppedL){
                                    while(!com_rechtCommand(0xff/2,0,0xff));//stop
                                    telPakket_L();
                                    if(aantalPakkettenTotaal>=MAXPAKKETTEN){
                                        operatingState = e_end;
                                        break;
                                    }
                                    stoppedL = 1;
                                }
                            }
                            else if(time<(detectTimeL+driveTimeToStop+1+DETECTCYCLERESETTIME)){//continue
                                if(stoppedL){
                                    while(!com_rechtCommand(0xff,DRIVESPEEDSCALED,0xff));
                                    stoppedL = 0;
                                }
                            }
                            else{
                                detect_L = 0;
                            }
                        }

                    }

                    if(detect_R){
                        if(stoppedL){//shift cycle start to compensate for stopping by other sensor
                            detectTimeR+=lastCycleDuration;
                        }
                        else{
                            if(time<(detectTimeR+driveTimeToStop)){
                                //do nothing
                            }
                            else if(time<(detectTimeR+driveTimeToStop+1)){//stop for 1 second and count
                                if(!stoppedR){
                                    while(!com_rechtCommand(0xff/2,0,0xff));//stop
                                    telPakket_R();
                                    if(aantalPakkettenTotaal>=MAXPAKKETTEN){
                                        operatingState = e_end;
                                        break;
                                    }
                                    stoppedR = 1;
                                }
                            }
                            else if(time<(detectTimeR+driveTimeToStop+1+DETECTCYCLERESETTIME)){//continue
                                if(stoppedR){
                                    while(!com_rechtCommand(0xff,DRIVESPEEDSCALED,0xff));
                                    stoppedR = 0;
                                }
                            }
                            else{
                                detect_R = 0;
                            }
                        }

                    }

                }
                }


                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_achteruit:{
                static float dirStartTime = 0;
                if(newSection){
                    dirStartTime = time;
                    while(!com_rechtCommand(0x00,DRIVESPEEDSCALED,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_links:{
                static float dirStartTime = 0;
                if(newSection){
                    dirStartTime = time;
                    while(!com_bochtCommand(e_links,DRIVESPEEDSCALED,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_rechts:{
                static float dirStartTime = 0;
                if(newSection){
                    dirStartTime = time;
                    while(!com_bochtCommand(e_rechts,DRIVESPEEDSCALED,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_blockblockL:{
                static float dirStartTime = 0;
                if(newSection){
                    dirStartTime = time;
                    while(!com_blokBlokCommand(e_links,DRIVESPEEDSCALED,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            case e_blockblockR:{
                static float dirStartTime = 0;
                if(newSection){
                    dirStartTime = time;
                    while(!com_blokBlokCommand(e_rechts,DRIVESPEEDSCALED,0xff));
                }

                if(com_agvDone){
                    nextSection = 1;
                }
                break;
            }
            }

            resetRoute = 0;
            break;
        }
        case e_end:{
            if(lastOperatingState!=e_end){
                while(!com_rechtCommand(0xff/2,0,0xff));//stop
            }

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

