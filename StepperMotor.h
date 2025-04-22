#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H


/*
Aansluitingen Motor 1:
    STEP      →  PA0 (Motor stappen aansturen)
    DIR       →  PA1 (Richting instellen)
*/

/*
Aansluitingen Motor 2:
    STEP      →  PB0 (Motor stappen aansturen)
    DIR       →  PB1 (Richting instellen)
*/

#include <avr/io.h>
#include <util/delay.h>

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

#define vol_snelheid 70
#define half_snelheid 30
#define stop_motor 0
#define half_snelheid_achteruit -30
#define vol_snelheid_achteruit -70

void stepperMotor_init(void);

void stepperMotor1_init(void);

void stepperMotor2_init(void);

void stepperMotor1_setSpeed(float speed);

void stepperMotor2_setSpeed(float speed);

// Functie om de AGV naar voren te laten rijden;
void rijVooruit(void);

// Functie om de AGV naar links te laten draaien
void draaiLinks(void);

// Functie om de AGV naar rechts te laten draaien
void draaiRechts(void);

// Functie om de AGV te laten stoppen
void stopAGV(void);


#endif // STEPPERMOTOR_H
