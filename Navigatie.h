#ifndef navigatie_included
#define navigatie_included

enum enum_richting{e_links = 0x01, e_rechts = 0x02, e_vooruit, e_achteruit};
typedef enum enum_richting t_richting;

extern double navigatie_speedGoal;
extern double navigatie_acceleratie;
extern double navigatie_afstandAfgelegd;
extern float navigatie_reverse;

void navigatie_setup();

void navigatie_navigeerPad();
void navigatie_navigeerMuurL(float afstand);
void navigatie_navigeerMuurR(float afstand);
void navigatie_navigeerBocht(t_richting f_draaiRichting, int f_radiusCm);
void navigatie_zetRichting(t_richting richting);
void stopAGV();

void navigatie_setSpeed(unsigned char f_speed);
void navigatie_setAcceleratie(float f_Acceleratie);

#endif // navigatie_included
