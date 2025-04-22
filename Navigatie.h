#ifndef navigatie_included
#define navigatie_included

enum e_richting{LINKS,RECHTS};
typedef enum e_richting t_richting;

void navigeer_pad();
void navigeer_bocht(t_richting draaiRichting);
void stopAGV();

#endif // navigatie_included
