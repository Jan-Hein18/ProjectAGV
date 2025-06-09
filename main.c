
#include "Knoppen.h"
#include "IRSensor.h"
#include "noodstop.h"
#include "clock.h"
#include "pakketten.h"
#include "com_module.h"
#include "lampjes.h"

#define DETECTCYCLERESETTIME 0.5 //min duration between end of detect cycle and a new cycle
#define STOPTIMEATPACKAGE 1 //how long to stop at a package
#define MAXWALLDISTANCE 15 // afstand tussen pads (niet gebruikt in deze code)


#define DRIVESPEED 0.125 //max 2
#define DRIVESPEEDSCALED DRIVESPEED*0xff/2 //scaled for use in command

#define PADAFSTAND 36

enum enum_richting{e_links = 0x01,e_rechts = 0x02, e_vooruit, e_achteruit, e_blockblockL, e_blockblockR};
typedef enum enum_richting t_richting;

#define ROUTELENGTE 4
const t_richting route[ROUTELENGTE] = {e_vooruit, e_blockblockR, e_vooruit, e_achteruit};

enum enum_operatingState{e_eStop, e_reset, e_idle, e_route, e_end};
typedef enum enum_operatingState t_operatingState;
t_operatingState operatingState = e_reset;

int main(void){
        // Variabelen voor het bijhouden van de vorige toestanden
    t_operatingState lastOperatingState = -1; //operating state in last cycle
    t_operatingState previousOperatingSate = -1; //operating state before last change
    t_operatingState currentOperatingState = -1; //operating state at start of cycle, do not change during cycle

    float cycleStartTime = time;

    int resetRoute = 1;

    while(1) {
        //calculate cycleTimes

        cycleStartTime = time;

        //buffer the previous operatingState when operatingState is changed
        currentOperatingState = operatingState;
        if(lastOperatingState!=operatingState){
            previousOperatingSate = lastOperatingState;
        }


        switch(currentOperatingState){
        case e_eStop:{
            static int continueOperation = 0;
 // Stoppen door stuurcommando (com_rechtCommand met 0 snelheid)
            while(!com_rechtCommand(0xff/2,0,0xff));//stop


// Start knop ingedrukt? Ga door of reset
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
                cycleStartTime = 0;

                //tellen
                initSensoren();
                initLamp();

                //ui
                knop_setup();

                //mi
                com_setup();

                //reset operatingStates
                resetRoute = 1;


                //--SET NEUTRAL STATE--
                while(!com_rechtCommand(0xff/2,0,0xff));//stop

            }



            //wacht tot geen knoppen ingedrukt
            if((!knop_ingedrukt(e_startKnop))){
                operatingState = e_idle;
            }

            break;
        }
        case e_idle:{
            if(lastOperatingState!=e_idle){
                while(!com_rechtCommand(0xff/2,0,0xff));//stop
            }


            if(knop_ingedrukt(e_startKnop)){
                operatingState = e_route;
            }
            break;
        }
        case e_route:{
 // Variabelen voor route-secties
    static int currentSection = 0;
    static float sectionStartTime = 0;
    static int sectionInitialized = 0;
    static float driveToPushTime = 0;

    switch (currentSection) {
        case 0: // Stap 1: rij snel vooruit
            if (!sectionInitialized) {
                com_rechtCommand(0xff, DRIVESPEEDSCALED, 0xff);
                sectionStartTime = time;
                sectionInitialized = 1;
            }
            // Ga door zodra IR potje detecteert
            if (IRSensor_links()) {
                driveToPushTime = time - sectionStartTime;  // Tijd meten voor later
                currentSection++;
                sectionInitialized = 0;
            }
            break;

        case 1: // Stap 4: rij langzaam vooruit + IR + switch
            if (!sectionInitialized) {
                com_rechtCommand(0x80, DRIVESPEEDSCALED, 0xff);
                sectionStartTime = time;
                sectionInitialized = 1;
            }

            lampjesSet(GEEL, 1); //geel aan

            if ((time - sectionStartTime >= 2.0) || knop_ingedrukt(e_limitSwitch)) {
                lampjesSet(GROEN, 1); //groen aan
                sectionStartTime = time;
                com_rechtCommand(0x00, DRIVESPEEDSCALED, 0xff);
                currentSection++;
                sectionInitialized = 0;
            }
            break;

        case 2: // Stap 6: wacht 3 sec met groen licht
            if ((time - sectionStartTime) >= 3.0) {
                    lampjesSet(GROEN, 0); // groen uit
                currentSection++;
                sectionInitialized = 0;
            }
            break;

        case 3: // Stap 7: achteruit S-bocht
            if (!sectionInitialized) {
                com_blokBlokCommand(0x01, DRIVESPEEDSCALED, 0xff);
                sectionInitialized = 1;
                lampjesSet(GEEL, 0); //geel uit
            }

            if (com_agvDone) {
                currentSection++;
                sectionInitialized = 0;
            }
            break;

        case 4: // Stap 8: rij vooruit
            if (!sectionInitialized) {
                com_rechtCommand(0xff, DRIVESPEEDSCALED, 0xff);
                sectionInitialized = 1;
            }

            if (IRSensor_links()) { // stap 9: detecteer potje
                currentSection++;
                sectionInitialized = 0;
            }
            break;

        case 5: // Stap 10: rij langzaam vooruit + IR + switch
            if (!sectionInitialized) {
                com_rechtCommand(0x80, DRIVESPEEDSCALED, 0xff);
                sectionStartTime = time;
                sectionInitialized = 1;
            }

            lampjesSet(GEEL, 1); // geel aan

            if ((time - sectionStartTime >= 2.0) || knop_ingedrukt(e_limitSwitch)) {
                lampjesSet(GROEN, 1); // groen aan
                sectionStartTime = time;
                com_rechtCommand(0x00, DRIVESPEEDSCALED, 0xff);
                currentSection++;
                sectionInitialized = 0;
            }
            break;

        case 6: // Stap 12: wacht 3 sec met groen licht
            if ((time - sectionStartTime) >= 3.0) {
                    lampjesSet(GROEN, 0);; // groen uit
                currentSection++;
                sectionInitialized = 0;
            }
            break;

        case 7: // Stap 13: recht achteruit, zelfde tijd als stap 0
            if (!sectionInitialized) {
                com_rechtCommand(0x00, DRIVESPEEDSCALED, 0xff);  // Achteruit
                sectionStartTime = time;
                lampjesSet(GEEL, 0); // geel uit
                sectionInitialized = 1;
            }

            if ((time - sectionStartTime) >= driveToPushTime) {
                com_rechtCommand(0xff / 2, 0, 0xff);  // Stop
                operatingState = e_end;
            }
            break;
    }

    break;
}

        case e_end:{
            if(lastOperatingState!=e_end){
                while(!com_rechtCommand(0xff/2,0,0xff));//stop
            }


            if(knop_ingedrukt(e_startKnop)){
                operatingState = e_reset;
            }
        }
        }
        lastOperatingState = currentOperatingState;
    }

    return 0;
}

