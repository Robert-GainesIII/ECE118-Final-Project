
// includes
#include "MotorControl.h"
#include "pwm.h"
#include "BOARD.h"
#include <xc.h>
#include "GlobalMacros.h"
#include "IO_Ports.h"
#include "AD.h"

//--------------------------
// tape sensing strategy:
// 1 pin for all leds using a darlington array
// takes in an ADC from a transresistive circuit from the tape sensors
// sets thresholds digitally
//--------------------------


// intializes tape sensor ports
// assumed AD has already been initalized
void TS_Init()
{
    // -- LED STUFF --
    // sets the output pin for toggling the all LEDs
    // initalizes the leds to off
    IO_PortsSetPortOutputs(TS_LEDS_CONTROL_PORT, TS_LEDS_PIN);
    IO_PortsClearPortBits(TS_LEDS_CONTROL_PORT, TS_LEDS_PIN);
    
    // -- TRANSRESISTIVE SENSOR ADC INPUT --
    AD_AddPins(TS_SIDE_L_PIN);
    AD_AddPins(TS_SIDE_C_PIN);
    AD_AddPins(TS_SIDE_R_PIN);
    
    AD_AddPins(TS_FLOOR_FRONT_PIN);
    AD_AddPins(TS_FLOOR_LEFT_PIN);
    AD_AddPins(TS_FLOOR_REAR_PIN);
    AD_AddPins(TS_FLOOR_RIGHT_PIN);
    
}

// sets all the led either on or off
void TS_SetLEDs(int status)
{
    if (status == LED_ON)
    {
        IO_PortsSetPortBits(TS_LEDS_CONTROL_PORT, TS_LEDS_PIN);
    }
    else
    {
        IO_PortsClearPortBits(TS_LEDS_CONTROL_PORT, TS_LEDS_PIN);
    }
}