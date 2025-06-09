#ifndef LAMPJES_H
#define LAMPJES_H

typedef enum {GEEL, GROEN} t_lamp;

void initLamp(void);
void lampjesSet(t_lamp kleur, int aan);
int lampjesStatus(t_lamp kleur);

#endif
