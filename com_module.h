#ifndef COM_MODULE_INCLUDED
#define COM_MODULE_INCLUDED


extern volatile int com_agvDone;
extern volatile int com_RXError;

void com_setup();

int com_sendCommand(unsigned char data[4]);
int com_rechtCommand(unsigned char f_arg, unsigned char f_speed, unsigned char f_acceleration);
int com_bochtCommand(unsigned char f_arg, unsigned char f_speed, unsigned char f_acceleration);
int com_blokBlokCommand(unsigned char f_arg, unsigned char f_speed, unsigned char f_acceleration);


#endif // COM_MODULE_INCLUDED
