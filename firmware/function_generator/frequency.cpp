#include "globals.h"

void updateMasterFrequency(){

masterFrequency = atol(freqStr);

if(masterFrequency < 1) masterFrequency = 1;

if(masterFrequency > 1000000) masterFrequency = 1000000;

}
