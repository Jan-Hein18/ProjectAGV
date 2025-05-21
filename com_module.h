#ifndef COM_MODULE_INCLUDED
#define COM_MODULE_INCLUDED


extern volatile int com_agvDone;
extern volatile int com_RXError;

void com_setup();

int com_sendCommand(char data[4]);
int com_rechtCommand(char f_arg, char f_speed, char f_acceleration);
int com_bochtCommand(char f_arg, char f_speed, char f_acceleration);
int com_blokBlokCommand(char f_arg, char f_speed, char f_acceleration);


#endif // COM_MODULE_INCLUDED
