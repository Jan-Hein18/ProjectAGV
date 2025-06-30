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


#define BLOKBLOKMODUS 1 //0 : eindig direct na S-bocht, 1 : rij achteruit tot in normaal pad, 2 : rij achteruit tot gelijk aan start positie, 3 : rij achteruit door gelijk aan padModus

#define MAXWALLDISTANCE 13

#define PADAFSTAND 36
#define PADBREEDTE 30
#define AGV_WIDTH ((float)20) //wheel to wheel [cm]




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
        if(newCommand){//&&!noodstop_ingedrukt()){
            com_command_temp.command = com_command.command;
            com_command_temp.arg = com_command.arg;
            com_command_temp.speed = com_command.speed;
            com_command_temp.acceleration = com_command.acceleration;
            switch(com_command_temp.command){
            case e_recht:{
                //while(!com_sendCommand(10));
                operatingState = e_pad;
                break;
            }
            case e_bocht:{
                operatingState = e_bochtState;
                break;
            }
            case e_blokBlok:{
                operatingState = e_blockBlock;
                break;
            }
            default:{
                //while(!com_sendCommand(50));
                operatingState = e_idle;
                break;
            }
            }
            newCommand = 0;
        }

        startOperatingState = operatingState;
        if(lastOperatingState!=operatingState){
            previousOperatingSate = lastOperatingState;
        }


        switch(startOperatingState){
        case e_eStop:{
            //com_sendCommand(2);
            stopAGV();
            break;
        }
        case e_reset:{
            //com_sendCommand(3);
            static int reset = 0;
            if(!reset){
                //--INITIALISATIE--
                //systeem
                initClock();
                noodstop_Setup();

                //navigatie
                ultrasoon_setup();
                stepperMotor_init();
                navigatie_setup();
                navigatie_setSpeed(3);
                navigatie_setAcceleratie(3);
                navigatie_zetRichting(e_vooruit);

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
            //if((!knop_ingedrukt(e_startKnop))&&(!knop_ingedrukt(e_plusKnop))&&(!knop_ingedrukt(e_minKnop))){
                reset = 0;
                operatingState = e_idle;
            //}

            break;
        }
        case e_idle:{
            if(lastOperatingState!=e_idle){while(!com_sendCommand(4));}
            stopAGV();

            break;
        }
        case e_pad:{
            //if(lastOperatingState!=e_pad){while(!com_sendCommand(5));}
            if((com_command_temp.arg>=(unsigned char)127)){
                navigatie_zetRichting(e_vooruit);

            }
            else{
                navigatie_zetRichting(e_achteruit);
            }

            //while(!com_sendCommand(com_command_temp.arg));
            navigatie_setSpeed(com_command_temp.speed);


            static int muurWegL = 0;
            static int muurWegR = 0;
            static float muurAfstand = 0;
            if(ultrasoon_wallGoneL()&&ultrasoon_wallGoneR()){
                while(!com_doneCommand());
                muurWegL = 0;
                muurWegR = 0;
                operatingState = e_idle;
            }
            else if(ultrasoon_wallGoneL()){
                if(!muurWegL){
                    muurWegL = 1;
                    muurAfstand = ultrasoon_getDistance_R();
                    if(muurAfstand>MAXWALLDISTANCE*0.75){
                        muurAfstand = MAXWALLDISTANCE*0.75;
                    }
                }
                muurWegR=0;
                navigatie_navigeerMuurR(muurAfstand);
            }
            else if(ultrasoon_wallGoneR()){
                if(!muurWegR){
                    muurWegR = 1;
                    muurAfstand = ultrasoon_getDistance_L();
                    if(muurAfstand>MAXWALLDISTANCE*0.75){
                        muurAfstand = MAXWALLDISTANCE*0.75;
                    }
                }
                muurWegL = 0;
                navigatie_navigeerMuurL(muurAfstand);
            }
            else{
                muurWegR = 0;
                muurWegL = 0;
                navigatie_navigeerPad();
            }
            break;
        }
        case e_bochtState:{
            //if(lastOperatingState!=e_bochtState){while(!com_sendCommand(6));}
            static double startAfstand = 0;
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

            navigatie_setSpeed(com_command_temp.speed);
            navigatie_navigeerBocht(com_command_temp.arg,(PADAFSTAND)/2);
            if((navigatie_afstandAfgelegd-startAfstand)>(3.14*(PADAFSTAND*0.01)/2)){
                while(!com_doneCommand());
                operatingState = e_idle;
            }

            break;
        }
        case e_blockBlock:{
            if(lastOperatingState!=e_blockBlock){while(!com_sendCommand(70));}
            static float startAfstand = 0;
            static float muurGatAfstand = 0;//afstand tussen start en het gat in de muur
            const float bochttussen = 0.06;
            static int terug = 0;
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
                terug = 0;

                while(!com_sendCommand(71));

                break;
            }
            };


            //navigatie_navigeerBocht(com_command_temp.arg,PADAFSTAND/2);

            if(!muurGatAfstand){
                navigatie_setSpeed(com_command_temp.speed);
                navigatie_navigeerPad();
                if((com_command_temp.arg==e_rechts)?ultrasoon_wallGoneR():ultrasoon_wallGoneL()){
                    muurGatAfstand = navigatie_afstandAfgelegd-startAfstand;
                }
                //while(!com_sendCommand(72));
            }
            else if((navigatie_afstandAfgelegd-(startAfstand+muurGatAfstand))<(3.14*(PADAFSTAND*0.01)/4)){//kwart rondje naar gat
                navigatie_setSpeed(com_command_temp.speed);
                navigatie_navigeerBocht((com_command_temp.arg==e_rechts)?e_rechts:e_links,PADAFSTAND/2);
                //while(!com_sendCommand(73));
            }
            else if((navigatie_afstandAfgelegd-(startAfstand+muurGatAfstand+(3.14*(PADAFSTAND*0.01)/4)))<(bochttussen)){
                navigatie_setSpeed(com_command_temp.speed);
                navigatie_navigeerBocht(e_vooruit,0);
            }
            else if((navigatie_afstandAfgelegd-(startAfstand+muurGatAfstand+bochttussen))<(3.14*(PADAFSTAND*0.01)/2)){//kwart rondje terug in pad
                navigatie_setSpeed(com_command_temp.speed);
                navigatie_navigeerBocht((com_command_temp.arg==e_rechts)?e_links:e_rechts,PADAFSTAND/2);
                //while(!com_sendCommand(74));
            }
            else{
                //while(!com_sendCommand(75));
                if(!terug&&navigatie_reverse){
                    navigatie_zetRichting(e_vooruit);
                    if(BLOKBLOKMODUS == 0){
                        navigatie_zetRichting(e_achteruit);
                    }
                    terug = 1;
                }
                else if(!terug){
                    navigatie_zetRichting(e_achteruit);
                    if(BLOKBLOKMODUS == 0){
                        navigatie_zetRichting(e_vooruit);
                    }
                    terug = 1;
                }
                navigatie_setSpeed(com_command_temp.speed);

                switch(BLOKBLOKMODUS){
                case 0:{//tot in pad
                    navigatie_navigeerBocht(e_vooruit,0);
                    if((!ultrasoon_wallGoneL())&&(!ultrasoon_wallGoneR())){
                        while(!com_doneCommand());
                        operatingState = e_idle;
                    }
                    break;
                }
                case 1:{//tot teruggereden in pad
                    if((!ultrasoon_wallGoneL())&&(!ultrasoon_wallGoneR())){//(((navigatie_afstandAfgelegd-(startAfstand+muurGatAfstand+bochttussen+(3.14*(PADAFSTAND*0.01)/2)))<(PADAFSTAND*0.01))&&((!ultrasoon_wallGoneL())&&(!ultrasoon_wallGoneR()))){
                        //navigatie_navigeerBocht(e_vooruit,0);
                        if(com_command_temp.arg==e_rechts){
                            navigatie_navigeerMuurR((PADBREEDTE-AGV_WIDTH)/2);
                        }
                        else{
                            navigatie_navigeerMuurL((PADBREEDTE-AGV_WIDTH)/2);
                        }
                    }
                    else{
                        while(!com_doneCommand());
                        operatingState = e_idle;
                    }
                    break;
                }
                case 2:{//tot bochtstralen en muurGatAfstand teruggereden en in pad
                    navigatie_navigeerBocht(e_vooruit,0);
                    if(((navigatie_afstandAfgelegd-startAfstand)<(3.14*(PADAFSTAND*0.01)+2*muurGatAfstand))&&((!ultrasoon_wallGoneL())&&(!ultrasoon_wallGoneR()))){
                        while(!com_doneCommand());
                        operatingState = e_idle;
                    }
                    break;
                }
                case 3:{//rij pad terug uit
                    if(((navigatie_afstandAfgelegd-(startAfstand+muurGatAfstand+bochttussen+(3.14*(PADAFSTAND*0.01)/2)))<(PADAFSTAND*0.01))){
                        navigatie_navigeerBocht(e_vooruit,0);
                    }
                    else if(!((ultrasoon_wallGoneL())&&(ultrasoon_wallGoneR()))){
                        navigatie_navigeerPad();
                    }
                    else{
                        while(!com_doneCommand());
                        operatingState = e_idle;
                    }
                    break;
                }
                default:{
                    while(!com_doneCommand());
                    operatingState = e_idle;
                }
                }
            }

            break;
        }
        case e_volg:{
            com_sendCommand(8);
            stopAGV();
            break;
        }
        }
        lastOperatingState = startOperatingState;
    }

    return 0;
}

