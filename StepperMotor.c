#include "stepperMotor.h"
#include "avr/io.h"

#define motor1_step_DDR     DDRB
#define motor1_step_port    PORTB
#define motor1_step_bit     (1<<PB6)

#define motor1_dir_DDR     DDRB
#define motor1_dir_port    PORTB
#define motor1_dir_bit     (1<<PB5)

#define motor2_step_DDR     DDRE
#define motor2_step_port    PORTE
#define motor2_step_bit     (1<<PE4)

#define motor2_dir_DDR     DDRE
#define motor2_dir_port    PORTE
#define motor2_dir_bit     (1<<PE5)


#define OCR1val_min 4000
#define OCR3val_min 4000

void stepperMotor_init(){
    stepperMotor1_init();
    stepperMotor2_init();
}


void stepperMotor1_init(){
    //--timer pwm--
    //step pin direction
    motor1_step_DDR |= motor1_step_bit;

    //wgm fast pwm
    TCCR1A |= (1<<WGM10)|(1<<WGM11);
    TCCR1B |= (1<<WGM12)|(1<<WGM13);

    //output OCR1B
    TCCR1A |= (1<<COM1B1);

    //prescaler 1
    TCCR1B |= (1<<CS10);

    //OCRA dutycycle lower than OCR1B to turn motor off
    OCR1A = OCR1val_min;

    //OCRA pulse 2us
    OCR1B = 32;


    //--dir pin--
    motor1_dir_DDR |= motor1_dir_bit;
}

void stepperMotor2_init(){
    //--timer pwm--
    //step pin direction
    motor2_step_DDR |= motor2_step_bit;

    //wgm fast pwm
    TCCR3A |= (1<<WGM30)|(1<<WGM31);
    TCCR3B |= (1<<WGM32)|(1<<WGM33);

    //output OCR1B
    TCCR3A |= (1<<COM3B1);

    //prescaler 1
    TCCR3B |= (1<<CS30);

    //OCRA dutycycle lower than OCR1B to turn motor off
    OCR3A = OCR3val_min;

    //OCRA pulse 2us
    OCR3B = 32;


    //--dir pin--
    motor2_dir_DDR |= motor2_dir_bit;
}



void stepperMotor1_setSpeed(float speed){
    //limiteer speed waarde
    speed = (speed>100)?100:speed;
    speed = (speed<-100)?-100:speed;

    //zet richting
    if(speed<0) {//achteruit
        motor1_dir_port &= ~motor1_dir_bit;
        speed*=-1;
    }
    else {//vooruit
        motor1_dir_port |= motor1_dir_bit;
    }


    //als waarde te klein zet motor uit
    if(speed < 7){
        OCR1A = 3;
    }
    else{
        //bereken top waarde en zet snelheid
        unsigned int ocr1Val = 100/(((double)1/65536)+((double)1/(1+OCR1val_min)-(double)1/65536)*speed);
        ocr1Val = (ocr1Val<OCR1val_min)?OCR1val_min:ocr1Val;
        OCR1A = ocr1Val;
    }
}

void stepperMotor2_setSpeed(float speed){
    //limiteer speed waarde
    speed = (speed>100)?100:speed;
    speed = (speed<-100)?-100:speed;

    //zet richting
    if(speed<0){//achteruit
        motor2_dir_port &= ~motor2_dir_bit;
        speed*=-1;
    }
    else{//vooruit
        motor2_dir_port |= motor2_dir_bit;
    }

    //als waarde te klein zet motor uit
    if(speed < 7){
        OCR3A = 3;
    }
    else{
        //bereken top waarde en zet snelheid
        unsigned int ocr3Val = 100/(((double)1/65536)+((double)1/(1+OCR1val_min)-(double)1/65536)*speed);
        ocr3Val = (ocr3Val<OCR3val_min)?OCR3val_min:ocr3Val;
        OCR3A = ocr3Val;
    }
}

void rijVooruit(void) {
    stepperMotor1_setSpeed(vol_snelheid);
    stepperMotor2_setSpeed(vol_snelheid);
}

void draaiLinks(void) {
    stepperMotor1_setSpeed(half_snelheid);
    stepperMotor2_setSpeed(vol_snelheid);
}

void draaiRechts(void) {
    stepperMotor1_setSpeed(vol_snelheid);
    stepperMotor2_setSpeed(half_snelheid);
}

void stopAGV(void) {
    stepperMotor1_setSpeed(stop_motor);
    stepperMotor2_setSpeed(stop_motor);
}
