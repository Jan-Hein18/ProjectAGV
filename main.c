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


#define BLOKBLOKMODUS 3 //0 : eindig direct na S-bocht, 1 : rij achteruit tot in normaal pad, 2 : rij achteruit tot gelijk aan start positie, 3 : rij achteruit door gelijk aan padModus

#define MAXWALLDISTANCE 15

#define PADAFSTAND 36

enum enum_operatingState{e_eStop, e_reset, e_idle, e_pad, e_bochtState, e_blockBlock, e_volg};
typedef enum enum_operatingState t_operatingState;
t_operatingState operatingState = e_reset;

int main(void){
    t_operatingState lastOperatingState = operatingState; //operating state in last cycle
    t_operatingState previousOperatingSate = operatingState; //operating state before last change
    t_operatingState startOperatingState = operatingState;
    while(1) {
        //buffer the previous operatingState when operatingState is changed
        static t_command com_command_temp = {0,0,0,0};
        if(newCommand&&!noodstop_ingedrukt()){
            com_command_temp = com_command;
            switch(com_command_temp.command){
            case e_recht:{
                operatingState = e_bocht;
                break;
            }
            case e_bocht:{
                operatingState = e_bocht;
                break;
            }
            case e_blokBlok:{
                operatingState = e_blockBlock;
                break;
            }
            default:{
                operatingState = e_idle;
                break;
            }
            }
        }

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
            navigatie_reverse = com_command_temp.arg<0x7F;
            navigatie_setSpeed(com_command_temp.speed);


            int muurWeg = 0;
            float muurAfstand = 0;
            if((ultrasoon_getDistance_L()>MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()>MAXWALLDISTANCE)){
                com_doneCommand();
                operatingState = e_idle;
            }
            else if(ultrasoon_getDistance_L()>MAXWALLDISTANCE){
                if(!muurWeg){
                    muurWeg = 1;
                    muurAfstand = ultrasoon_getDistance_R();
                }
                navigatie_navigeerMuurR(muurAfstand);
            }
            else if(ultrasoon_getDistance_R()>MAXWALLDISTANCE){
                if(!muurWeg){
                    muurWeg = 1;
                    muurAfstand = ultrasoon_getDistance_L();
                }
                navigatie_navigeerMuurL(muurAfstand);
            }
            else{
                muurWeg = 0;
                navigatie_navigeerPad();
            }
            break;
        }
        case e_bochtState:{
            static float startAfstand = 0;
            switch(lastOperatingState){
            case e_bochtState:
            case e_eStop:{
                break;
            }
            default:{
                startAfstand = navigatie_afstandAfgelegd;
                break;
            }
            };

            navigatie_setSpeed(com_command.speed);
            navigatie_navigeerBocht(com_command.arg,PADAFSTAND/2);
            if((navigatie_afstandAfgelegd-startAfstand)>(3.14*(PADAFSTAND*0.01)/2)){
                com_doneCommand();
                operatingState = e_idle;
            }

            break;
        }
        case e_blockBlock:{
            static float startAfstand = 0;
            static float muurGatAfstand = 0;//afstand tussen start en het gat in de muur
            static float reverse = 0;
            switch(lastOperatingState){
            case e_blockBlock:
            case e_eStop:{
                //geen reset
                break;
            }
            default:{
                //reset
                startAfstand = navigatie_afstandAfgelegd;
                muurGatAfstand = 0;
                reverse = navigatie_reverse;
                break;
            }
            };

            navigatie_navigeerBocht(com_command.arg,PADAFSTAND/2);

            if(!muurGatAfstand){
                navigatie_setSpeed(com_command.speed);
                navigatie_navigeerPad();
                if(((com_command.arg==e_rechts)?ultrasoon_getDistance_R():ultrasoon_getDistance_L())>MAXWALLDISTANCE){
                    muurGatAfstand = navigatie_afstandAfgelegd-startAfstand;
                }
            }
            if((navigatie_afstandAfgelegd-(startAfstand+muurGatAfstand))<(3.14*(PADAFSTAND*0.01)/4)){//kwart rondje naar gat
                navigatie_setSpeed(com_command.speed);
                navigatie_navigeerBocht((com_command.arg==e_rechts)?e_rechts:e_links,PADAFSTAND/2);

            }
            else if((navigatie_afstandAfgelegd-(startAfstand+muurGatAfstand))<(3.14*(PADAFSTAND*0.01)/2)){//kwart rondje terug in pad
                navigatie_setSpeed(com_command.speed);
                navigatie_navigeerBocht((com_command.arg==e_rechts)?e_links:e_rechts,PADAFSTAND/2);
            }
            else{
                navigatie_reverse = !reverse;
                navigatie_setSpeed(com_command.speed);

                switch(BLOKBLOKMODUS){
                case 0:{//tot in pad
                    navigatie_navigeerBocht(e_vooruit,0);
                    if((ultrasoon_getDistance_L()<MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()<MAXWALLDISTANCE)){
                        com_doneCommand();
                        operatingState = e_idle;
                    }
                    break;
                }
                case 1:{//tot bochtstralen teruggereden en in pad
                    navigatie_navigeerBocht(e_vooruit,0);
                    if(((navigatie_afstandAfgelegd-startAfstand)<(3.14*(PADAFSTAND*0.01)+muurGatAfstand))&&(ultrasoon_getDistance_L()<MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()<MAXWALLDISTANCE)){
                        com_doneCommand();
                        operatingState = e_idle;
                    }
                    break;
                }
                case 2:{//tot bochtstralen en muurGatAfstand teruggereden en in pad
                    navigatie_navigeerBocht(e_vooruit,0);
                    if(((navigatie_afstandAfgelegd-startAfstand)<(3.14*(PADAFSTAND*0.01)+2*muurGatAfstand))&&(ultrasoon_getDistance_L()<MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()<MAXWALLDISTANCE)){
                        com_doneCommand();
                        operatingState = e_idle;
                    }
                    break;
                }
                case 3:{//rij pad terug uit
                    if(!(((navigatie_afstandAfgelegd-startAfstand)<(3.14*(PADAFSTAND*0.01)+2*muurGatAfstand))&&(ultrasoon_getDistance_L()<MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()<MAXWALLDISTANCE))){
                        navigatie_navigeerBocht(e_vooruit,0);
                    }
                    else if(!((ultrasoon_getDistance_L()>MAXWALLDISTANCE)&&(ultrasoon_getDistance_R()>MAXWALLDISTANCE))){
                        navigatie_navigeerPad();
                    }
                    else{
                        com_doneCommand();
                        operatingState = e_idle;
                    }
                    break;
                }
                default:{
                    com_doneCommand();
                    operatingState = e_idle;
                }
                }
            }

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

