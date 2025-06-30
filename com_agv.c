#include "com_agv.h"

#include <avr/io.h>
#include <avr/interrupt.h>

//dit is voor usart0, dus de rx0(PE0) en tx0(PE1) pin, usart0 is ook via usb mee te communiceren


#define BAUDRATE 9600
#define FRAMEFORMAT 8   //5 to 9
#define USARTMODE 0     //0 : asynchronous, 1 : synchronous doubleSpeed, 2 : synchronous slave, 3 : synchronous master
#define PARITY 0        //0 : off, 2 : even, 3 : odd
#define POLARITY 0      //0 : rising tx falling rx, 1 : falling tx rising rx
#define STOPBITS 1      //1, 2
#define DOUBLESPEED 0   //0 : off, 1 : on




volatile int com_RXError = 0;
volatile int newCommand = 0;
volatile t_command com_command = {0, 0, 0, 0};
static volatile char rxBuffer[4] = {0,0,0,0};
ISR(USART0_RX_vect){//data recieved
    static int bufferCount = 0;
    if(UCSR0A&((1<<FE0)|(1<<DOR0)|(1<<UPE0))){
        com_RXError = 1;

        //room for custom rx error handling
    }

    volatile char recievedData = UDR0;

    rxBuffer[bufferCount] = recievedData;

    bufferCount++;
    if(bufferCount>=4){
        bufferCount = 0;

        com_command.command = rxBuffer[0];
        com_command.arg = rxBuffer[1];
        com_command.speed = rxBuffer[2];
        com_command.acceleration = rxBuffer[3];

        newCommand = 1;



        //room for custom execution when command is fully recieved
    }
}



void com_setup(){
    //disable interrupts
    UCSR0B &=  ~(1<<RXCIE0);

    //disable rx and tx
    UCSR0B &= ~(1<<RXEN0);
    UCSR0B &= ~(1<<TXEN0);

    //disable power saving
    PRR0 &= ~(1<<PRUSART0);

    //usart mode and baudrade (not adjusted for rounding)
    #if (USARTMODE==0)
        UCSR0C &= ~(1<<UMSEL00);
        UCSR0C &= ~(1<<UMSEL01);
        UBRR0 = (F_CPU/((unsigned long)16*BAUDRATE)-1);
    #elif (USARTMODE==1)
        UCSR0C &= ~(1<<UMSEL00);
        UCSR0C &= ~(1<<UMSEL01);
        UBRR0 = (F_CPU/((unsigned int)8*BAUDRATE)+1);
    #elif (USARTMODE==2)
        UCSR0C |= (1<<UMSEL00);
        UCSR0C &= ~(1<<UMSEL01);
        UBRR0 = 0;
        DDRE &= ~(1<<PE2);
    #elif (USARTMODE==3)
        UCSR0C |= (1<<UMSEL00);
        UCSR0C &= ~(1<<UMSEL01);
        UBRR0 = (F_CPU/((unsigned int)2*BAUDRATE)+1);
        DDRE |= (1<<PE2);
    #endif // USARTMODE


    //double speed mode
    #if (DOUBLESPEED==1)&&(USARTMODE==1)
        UCSR0A |= (1<<U2X0);
    #else
        UCSR0A &= ~(1<<U2X0);
    #endif // DOUBLESPEED


    //synchronous clock sample edge
    #if (POLARITY==1)&&((USARTMODE==2)||(USARTMODE==3))
        UCSR0C |= (1<<UCPOL0);
    #elif ((USARTMODE==2)||(USARTMODE==3))
        UCSR0C &= ~(1<<UCPOL0);
    #endif // POLARITY


    //parity
    if((PARITY)&(1<<0)){ // bit0
        UCSR0C |= (1<UPM00);
    }
    else{
        UCSR0C &= ~(1<UPM00);
    }

    if((PARITY)&(1<<1)){ // bit1
        UCSR0C |= (1<UPM01);
    }
    else{
        UCSR0C &= ~(1<UPM01);
    }


    //stop bits
    #if (STOPBITS==1)
        UCSR0C &= ~(1<<USBS0);
    #else
        UCSR0C |= (1<<USBS0);
    #endif // STOPBITS


    //frame length setting
    #if (FRAMEFORMAT==9)
        UCSR0B |= (1<UCSZ02);
        UCSR0C |= (1<UCSZ00);
        UCSR0C |= (1<UCSZ01);
    #else
        UCSR0B &= ~(1<UCSZ02);

        if((FRAMEFORMAT-1)&(1<<0)){ // bit0
            UCSR0C |= (1<UCSZ00);
        }
        else{
            UCSR0C &= ~(1<UCSZ00);
        }

        if((FRAMEFORMAT-1)&(1<<1)){ // bit1
            UCSR0C |= (1<UCSZ01);
        }
        else{
            UCSR0C &= ~(1<UCSZ01);
        }
    #endif // FRAMEFORMAT


    //reset error bits
    UCSR0A &= ~((1<<FE0)|(1<<DOR0)|(1<<UPE0));


    //enable interrupts
    UCSR0B |=  (1<<RXCIE0);
    sei();


    //enable rx and tx
    UCSR0B |= (1<<RXEN0);
    UCSR0B |= (1<<TXEN0);
}

int com_sendCommand(char data){
    if(UCSR0A&(1<<UDRE0)){
        UDR0 = data;
        return 1;
    }
    else{//return 0 when data can not be send
        return 0;
    }
}

int com_doneCommand(){
    return com_sendCommand(0x01); //return 0 when data can not be send
}




