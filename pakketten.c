#include "pakketten.h"
#include "metaalDetector.h"

unsigned int aantalPakkettenTotaal = 0;
unsigned int aantalPakkettenMetaal = 0;
unsigned int aantalPakkettenLeeg = 0;


void telPakket_L(){
    if(metaaldetector_links()){
        aantalPakkettenMetaal++;
    }
    else{
        aantalPakkettenLeeg++;
    }

    aantalPakkettenTotaal++;
}
