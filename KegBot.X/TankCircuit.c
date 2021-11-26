
// includes
#include "TankCircuit.h"
#include <pwm.h>
#include <BOARD.h>
#include <xc.h>
#include "GlobalMacros.h"
#include <IO_Ports.h>


//returns ERROR is initilization of pin was unsuccessful
char Tank_Init(void){
     return AD_AddPins(TRACKWIRE_PIN);
}


//returns ERROR if read was not made, else returns 10 bit unsigned adc value
unsigned int TankRead(void){
    return AD_ReadADPin(TRACKWIRE_PIN);
    
}