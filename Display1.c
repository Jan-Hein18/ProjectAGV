#include "Display1.h"
#include "IRSensor.h"
#include <avr/io.h>
#include <util/delay.h>


#define SCLK_DDR DDRF
#define LCLK_DDR DDRF
#define DATA_DDR DDRF

#define SCLK_PORT PORTF
#define LCLK_PORT PORTF
#define DATA_PORT PORTF

#define SCLK_PIN (1<<6)
#define LCLK_PIN (1<<7)
#define DATA_PIN (1<<5)

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


unsigned char letters [26] = {
  0b01110111,//A
  0b01111100,//B
  0b00111001,//C
  0b01011110,//D
  0b01111001,//E
  0b01110001,//F
  0b00111101,//G
  0b01110100,//H
  0b00000110,//I
  0b00011110,//J
  0b01110101,//K
  0b00111000,//L
  0b01010101,//M
  0b01010100,//N
  0b01011100,//O
  0b01110011,//P
  0b01100111,//Q
  0b01010000,//R
  0b01101101,//S
  0b01111000,//T
  0b00111110,//U
  0b00011100,//V
  0b01101010,//W
  0b01110110,//X
  0b01101110,//Y
  0b01011011,//Z
};


void display_letter(char letter, unsigned char digit){
    if((65<=letter)&&(letter<=90)){
        _7segment_write(letters[letter-65],digit);
    }
    else if((97<=letter)&&(letter<=122)){
        _7segment_write(letters[letter-97],digit);
    }
    else if(letter==32){
        _7segment_write(0,digit);
    }
}

void display_string(char _string[4]){
    for(int i = 0;i<4;i++){
        display_letter(_string[i],i);
        _delay_ms(1);
    }
    _7segment_write(0,0);
}


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

void display_getal(unsigned int getal) {
    for (int i = 3; i > -1; i--){
        _7segment_write(numbers[getal%10], i);
        getal /= 10;
        _delay_ms(1);
    }
    _7segment_write(0,0);
}

