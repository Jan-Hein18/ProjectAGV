#include "Navigatie.h"
#include <avr/io.h>

#include "StepperMotor.h"
#include "Ultrasoon.h"


#define AGV_WIDTH 25 //wheel to wheel cm

#define PAD_DEADZONE 0.1 //cm
#define PAD_SPEED 75 //%
#define PAD_TURN_FACTOR 0.5

#define BOCHT_RADIUS 30 //cm
#define BOCHT_SPEED_OUTER 75 //%
#define BOCHT_SPEED_INNER (((2*BOCHT_RADIUS-AGV_WIDTH)/(2*BOCHT_RADIUS+AGV_WIDTH))*BOCHT_SPEED_OUTER) //%


void stopAGV() {
    stepperMotor1_setSpeed(0);
    stepperMotor2_setSpeed(0);
}


void navigeer_pad(){
    float afstand_links = ultrasoon_getDistance_L();
    float afstand_rechts = ultrasoon_getDistance_R();

    if(afstand_rechts > afstand_links+PAD_DEADZONE){//draai naar rechts
        stepperMotor1_setSpeed(PAD_SPEED);
        stepperMotor1_setSpeed(PAD_SPEED*PAD_TURN_FACTOR);
    }
    else if(afstand_links > afstand_rechts+PAD_DEADZONE){//draai naar links
        stepperMotor1_setSpeed(PAD_SPEED*PAD_TURN_FACTOR);
        stepperMotor1_setSpeed(PAD_SPEED);
    }
}

void navigeer_bocht(t_richting draaiRichting){
    switch(draaiRichting){
    case LINKS:{
        stepperMotor1_setSpeed(BOCHT_SPEED_INNER);
        stepperMotor1_setSpeed(BOCHT_SPEED_OUTER);
        break;
    }
    case RECHTS:{
        stepperMotor1_setSpeed(BOCHT_SPEED_OUTER);
        stepperMotor1_setSpeed(BOCHT_SPEED_INNER);
        break;
    }
    }
}
