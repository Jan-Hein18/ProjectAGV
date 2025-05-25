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

void telPakket_R(){
    if(metaaldetector_rechts()){
        aantalPakkettenMetaal++;
    }
    else{
        aantalPakkettenLeeg++;
    }

    aantalPakkettenTotaal++;
}
