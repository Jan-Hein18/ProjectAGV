#include "pakketten.h"
#include "IRSensor.h"

unsigned int aantalPakketten = 0;


void telPakketten(){
    {//links
        static int IRActief = 0;
        if(!IRActief&&IRSensor_links()){
            IRActief = 1;
            aantalPakketten++;
        }
        else if(IRActief&&!IRSensor_links()){
            IRActief = 0;
        }

    }


}
