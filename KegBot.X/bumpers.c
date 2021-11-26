
// includes
#include "MotorControl.h"
#include <pwm.h>
#include <BOARD.h>
#include <xc.h>
#include "GlobalMacros.h"
#include <IO_Ports.h>


// initializes bumper ports
void Bumper_Init(void)
{
    // sets the read pins for the 4 bumpers
    int pattern = 0x0;
    pattern |= FL_BUMPER_PIN;
    pattern |= FR_BUMPER_PIN;
    pattern |= RL_BUMPER_PIN;
    pattern |= RR_BUMPER_PIN;
    
    IO_PortsSetPortInputs(BUMPER_CONTROL_PORT, pattern);
}

// front left bumper
// returns a boolean if the bumper is currently pressed
int FLBumperIsPressed(void)
{
    return ( (IO_PortsReadPort(BUMPER_CONTROL_PORT) & FL_BUMPER_PIN) != 0 );
}

// front right bumper
// returns a boolean if the bumper is currently pressed
int FRBumperIsPressed(void)
{
    return ( (IO_PortsReadPort(BUMPER_CONTROL_PORT) & FR_BUMPER_PIN) != 0 );
}

// rear left bumper
// returns a boolean if the bumper is currently pressed
int RLBumperIsPressed(void)
{
    return ( (IO_PortsReadPort(BUMPER_CONTROL_PORT) & RL_BUMPER_PIN) != 0 );
}

// rear right bumper
// returns a boolean if the bumper is currently pressed
int RRBumperIsPressed(void)
{
    return ( (IO_PortsReadPort(BUMPER_CONTROL_PORT) & RR_BUMPER_PIN) != 0 );
}