#include "Display1.h"
#include "IRSensor.h"
#include <avr/io.h>


#define SCLK_DDR DDRF
#define LCLK_DDR DDRF
#define DATA_DDR DDRF

#define SCLK_PORT PORTF
#define LCLK_PORT PORTF
#define DATA_PORT PORTF

#define SCLK_PIN (1<<6)
#define LCLK_PIN (1<<7)
#define DATA_PIN (1<<5)

int teller = 0;
int verwachtAantalPakketten = 0;

unsigned char numbers [10] = {
  0b00111111,
  0b00000110,
  0b01011011,
  0b01001111,
  0b01100110,
  0b01101101,
  0b01111101,
  0b00000111,
  0b01111111,
  0b01101111
};


unsigned char letters [7] = {
  0b00111111,
  0b01110111,
  0b01111100,
  0b00111001,
  0b01011110,
  0b01111001,
  0b01110001,
};





void _7segment_setup(){
  SCLK_DDR |= SCLK_PIN;
  LCLK_DDR |= LCLK_PIN;
  DATA_DDR |= DATA_PIN;
}

void _7segment_write(unsigned char data, unsigned char digit){
  LCLK_PORT &= ~LCLK_PIN;
  for(int i=7; i>=0;i--){
    SCLK_PORT &= ~SCLK_PIN;
    DATA_PORT &= ~DATA_PIN;
    DATA_PORT |= (!((1<<(4+digit))&(1<<i)))*DATA_PIN;
    SCLK_PORT |= SCLK_PIN;
  }
  for(int i=7; i>=0;i--){
    SCLK_PORT &= ~SCLK_PIN;
    DATA_PORT &= ~DATA_PIN;
    DATA_PORT |= (!!(data&(1<<i)))*DATA_PIN;
    SCLK_PORT |= SCLK_PIN;
  }
  LCLK_PORT |= LCLK_PIN;
}

void display(unsigned int getal) {
    for (int i = 3; i > -1; i--){
        _7segment_write(numbers[getal%10], i);
        getal /= 10;
        _delay_ms(1);
    }
    _7segment_write(0,0);
}

void displayGeteldAantalPakketten() {
    display(teller * 100 + verwachtAantalPakketten);
}

void displayAantalPakketten(void)
{
    display(teller * 100 + verwachtAantalPakketten);
}

