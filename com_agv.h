#ifndef COM_MODULE_INCLUDED
#define COM_MODULE_INCLUDED

enum enum_commands{e_recht = 0x01, e_bocht = 0x02, e_blokBlok = 0x03};
typedef enum enum_commands t_commands;

struct struct_command{
    t_commands command;
    char arg;
    char speed;
    char acceleration;
};
typedef struct struct_command t_command;

extern volatile t_command com_command;
extern volatile int com_RXError;

extern volatile int newCommand;

void com_setup();

int com_doneCommand();




#endif // COM_MODULE_INCLUDED
