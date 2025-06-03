#include <IRSensor.h>
#include <Knoppen.h>


#define LAMPREGISTER DDRB
#define LAMPPORT PORTB
#define LAMPGROEN PB6
#define LAMPGEEL PB5

void initLamp(void){
    LAMPREGISTER &= ~((1<<LAMPGEEL) | (1<<LAMPGROEN));
    LAMPPORT &= ~((1<<LAMPGEEL) | (1<<LAMPGROEN));
}

void lampjesToggle(void){
    int groenAan = 0;
    int geelAan = 0;
    if((IRSensorActief==1) && (geelAan == 0)){
        geelAan = 1;
        LAMPPORT |= (1<<LAMPGEEL)
    }
   if((IRSensorActief==0) && (geelAan == 1)){
        geelAan = 0;
        LAMPPORT &= ~(1<<LAMPGEEL)
   }
   if(knop_ingedrukt(e_limitSwitch) && groenAan==0){
        groenAan = 1;
        LAMPPORT |= (1<<LAMPGROEN);
   }
   if(!(knop_ingedrukt(e_limitSwitch)) && groenAan = 1){
        groenAan = 0;
        LAMPPORT &= ~(1<<LAMPGROEN);
   }
}
