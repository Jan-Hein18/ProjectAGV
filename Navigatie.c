#include "Navigatie.h"

#include <avr/io.h>

#include "StepperMotor.h"
#include "Ultrasoon.h"
#include "clock.h"


#define AGV_WIDTH 25 //wheel to wheel [cm]
#define MAX_SPEED 2 // [m/s]
#define MAX_ACCELERATIE 2 // [m/s]
#define MIN_RADIUS (AGV_WIDTH/2) //[cm]
#define MOTORCONSTANT 0.1 //[m/(s*percent)]
#define TURNCONSTANT 10 //turn radius when difference between wall distances is 1cm [cm]
#define PAD_TURNDEADZONE 0.5 //[cm]



float navigatie_speedGoal = 0;
float navigatie_speedCurrent = 0;
float navigatie_acceleratie = 0;
float navigatie_acceleratieTijd = 0;

void navigatie_setup(){
    navigatie_speedGoal = 0;
    navigatie_speedCurrent = 0;
    navigatie_acceleratie = 0;
    navigatie_acceleratieTijd = time;
}


void stopAGV() {
    stepperMotor1_setSpeed(0);
    stepperMotor2_setSpeed(0);
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
}

void navigatie_navigeerBocht(t_richting f_draaiRichting, int f_radiusCm){
    //bereken snelheid
    navigatie_speedCurrent += navigatie_acceleratie*(time-navigatie_acceleratieTijd);
    if(navigatie_speedCurrent>navigatie_speedGoal){
        navigatie_speedCurrent = navigatie_speedGoal;
    }
    navigatie_acceleratieTijd = time;


    if(f_radiusCm<MIN_RADIUS){//limiteer radius
        f_radiusCm = MIN_RADIUS;
    }

    switch(f_draaiRichting){
    case e_links:{
        stepperMotor1_setSpeed(((2*f_radiusCm-AGV_WIDTH)/(2*f_radiusCm+AGV_WIDTH))*navigatie_speedCurrent);
        stepperMotor2_setSpeed(navigatie_speedCurrent);
        break;
    }
    case e_rechts:{
        stepperMotor1_setSpeed(navigatie_speedCurrent);
        stepperMotor2_setSpeed(((2*f_radiusCm-AGV_WIDTH)/(2*f_radiusCm+AGV_WIDTH))*navigatie_speedCurrent);
        break;
    }
    case e_vooruit:{
        stepperMotor1_setSpeed(MOTORCONSTANT*navigatie_speedCurrent);
        stepperMotor2_setSpeed(MOTORCONSTANT*navigatie_speedCurrent);
        break;
    }
    default:{//stop zsm
        stepperMotor1_setSpeed(0);
        stepperMotor2_setSpeed(0);
        navigatie_speedCurrent = 0;
        break;
    }
    }
}


void navigatie_setSpeed(float f_speed){
    navigatie_speedGoal = (f_speed<MAX_SPEED)?f_speed:MAX_SPEED;
}

void navigatie_setAcceleratie(float f_Acceleratie){
    navigatie_acceleratie = (f_Acceleratie<MAX_ACCELERATIE)?f_Acceleratie:MAX_ACCELERATIE;
}
