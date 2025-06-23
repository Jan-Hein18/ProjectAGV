#include "Navigatie.h"

#include <avr/io.h>

#include "StepperMotor.h"
#include "Ultrasoon.h"
#include "clock.h"
#include "Display1.h"


#define AGV_WIDTH 18.5 //wheel to wheel [cm]
#define MAX_SPEED 0.2 // [m/s]
#define MAX_ACCELERATIE 1 // [m/s2]
#define MIN_RADIUS (AGV_WIDTH/2) //[cm]
#define MOTORCONSTANT 425 //[percent/(m/s)]
#define TURNCONSTANT 1 //turn radius when difference between wall distances is 1cm [cm]
#define PAD_TURNDEADZONE 0.5 //[cm]



float navigatie_speedGoal = 0;
static float navigatie_speedCurrent = 0;
float navigatie_acceleratie = 0;
float navigatie_reverse = 0;
static float navigatie_executieTijd = 0;

float navigatie_afstandAfgelegd = 0;

void navigatie_setup(){
    navigatie_speedGoal = 0;
    navigatie_speedCurrent = 0;
    navigatie_acceleratie = MAX_ACCELERATIE;
    navigatie_afstandAfgelegd = 0;
    navigatie_executieTijd = time;
}


void navigatie_zetRichting(t_richting richting){
    if(richting == e_achteruit){
        navigatie_reverse = 0;
        ultrasoon_switchSensor(e_ultrasooonAchter);

    }
    else{
        navigatie_reverse = 1;
        ultrasoon_switchSensor(e_ultrasooonVoor);
    }
}


void stopAGV() {
    stepperMotor1_setSpeed(0);
    stepperMotor2_setSpeed(0);
    navigatie_setSpeed(0);
    navigatie_speedCurrent = 0;
}

void navigatie_navigeerPad(){
    float afstand_links = ultrasoon_getDistance_L();
    float afstand_rechts = ultrasoon_getDistance_R();

    if(afstand_rechts > afstand_links+PAD_TURNDEADZONE){//draai naar rechts
        navigatie_navigeerBocht(e_rechts,(1.0/(afstand_rechts-afstand_links))*TURNCONSTANT);
    }
    else if(afstand_links > afstand_rechts+PAD_TURNDEADZONE){//draai naar links
        navigatie_navigeerBocht(e_links,(1.0/(afstand_links-afstand_rechts))*TURNCONSTANT);
    }
    else{
        navigatie_navigeerBocht(e_vooruit,0);
    }
    //navigatie_navigeerBocht(e_vooruit,0);
}

void navigatie_navigeerMuurL(float afstand){
    float afstand_links = ultrasoon_getDistance_L();
    float afstand_rechts = afstand+(afstand-afstand_links);

    if(afstand_rechts > afstand_links+PAD_TURNDEADZONE){//draai naar rechts
        navigatie_navigeerBocht(e_rechts,(1.0/(afstand_rechts-afstand_links))*TURNCONSTANT);
    }
    else if(afstand_links > afstand_rechts+PAD_TURNDEADZONE){//draai naar links
        navigatie_navigeerBocht(e_links,(1.0/(afstand_links-afstand_rechts))*TURNCONSTANT);
    }
    else{
        navigatie_navigeerBocht(e_vooruit,0);
    }
}

void navigatie_navigeerMuurR(float afstand){
    float afstand_rechts = ultrasoon_getDistance_R();
    float afstand_links = afstand+(afstand-afstand_rechts);

    if(afstand_rechts > afstand_links+PAD_TURNDEADZONE){//draai naar rechts
        navigatie_navigeerBocht(e_rechts,(1.0/(afstand_rechts-afstand_links))*TURNCONSTANT);
    }
    else if(afstand_links > afstand_rechts+PAD_TURNDEADZONE){//draai naar links
        navigatie_navigeerBocht(e_links,(1.0/(afstand_links-afstand_rechts))*TURNCONSTANT);
    }
    else{
        navigatie_navigeerBocht(e_vooruit,0);
    }
}

void navigatie_navigeerBocht(t_richting f_draaiRichting, int f_radiusCm){
    //update afstand
    static float lastRealSpeed = 0; //compenseer voor snelheid in bochten
    navigatie_afstandAfgelegd += lastRealSpeed*(time-navigatie_executieTijd);

    //bereken snelheid
    if(navigatie_speedCurrent<navigatie_speedGoal){
        navigatie_speedCurrent += navigatie_acceleratie*(time-navigatie_executieTijd);
        if(navigatie_speedCurrent>navigatie_speedGoal){
            navigatie_speedCurrent = navigatie_speedGoal;
        }
    }
    else if(navigatie_speedCurrent>navigatie_speedGoal){
        navigatie_speedCurrent -= navigatie_acceleratie*(time-navigatie_executieTijd);
        if(navigatie_speedCurrent<0){
            navigatie_speedCurrent = 0;
        }
    }
    navigatie_executieTijd = time;


    if(f_radiusCm<MIN_RADIUS){//limiteer radius
        f_radiusCm = MIN_RADIUS;
    }

    switch(f_draaiRichting){
    case e_links:{
        stepperMotor1_setSpeed(MOTORCONSTANT*((2*f_radiusCm-AGV_WIDTH)/(2*f_radiusCm+AGV_WIDTH))*navigatie_speedCurrent*((navigatie_reverse)?-1:1));
        stepperMotor2_setSpeed(MOTORCONSTANT*navigatie_speedCurrent*((navigatie_reverse)?-1:1));
        lastRealSpeed = ((2*f_radiusCm)/(2*f_radiusCm+AGV_WIDTH))*navigatie_speedCurrent;
        break;
    }
    case e_rechts:{
        stepperMotor1_setSpeed(MOTORCONSTANT*navigatie_speedCurrent*((navigatie_reverse)?-1:1));
        stepperMotor2_setSpeed(MOTORCONSTANT*(((2*f_radiusCm-AGV_WIDTH)/(2*f_radiusCm+AGV_WIDTH))*navigatie_speedCurrent)*((navigatie_reverse)?-1:1));
        lastRealSpeed = ((2*f_radiusCm)/(2*f_radiusCm+AGV_WIDTH))*navigatie_speedCurrent;
        break;
    }
    case e_vooruit:{
        stepperMotor1_setSpeed(MOTORCONSTANT*navigatie_speedCurrent*((navigatie_reverse)?-1:1));
        stepperMotor2_setSpeed(MOTORCONSTANT*navigatie_speedCurrent*((navigatie_reverse)?-1:1));
        lastRealSpeed = navigatie_speedCurrent;
        break;
    }
    default:{//stop zsm
        stepperMotor1_setSpeed(0);
        stepperMotor2_setSpeed(0);
        navigatie_speedCurrent = 0;
        lastRealSpeed = 0;
        break;
    }
    }
}


void navigatie_setSpeed(float f_speed){
    navigatie_speedGoal = (f_speed<MAX_SPEED)?((f_speed<0)?0:f_speed):MAX_SPEED;
}

void navigatie_setAcceleratie(float f_Acceleratie){
    navigatie_acceleratie = (f_Acceleratie<MAX_ACCELERATIE)?((f_Acceleratie<0)?0:f_Acceleratie):MAX_ACCELERATIE;
}
