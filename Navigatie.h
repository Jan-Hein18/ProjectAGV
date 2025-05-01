#ifndef navigatie_included
#define navigatie_included

enum enum_richting{e_links,e_rechts, e_vooruit};
typedef enum enum_richting t_richting;

extern float navigatie_speedGoal;
extern float navigatie_acceleratie;

void navigatie_setup();

void navigatie_navigeerPad();
void navigatie_navigeerBocht(t_richting f_draaiRichting, int f_radiusCm);
void stopAGV();

void navigatie_setSpeed(float f_speed);
void navigatie_setAcceleratie(float f_Acceleratie);

#endif // navigatie_included
