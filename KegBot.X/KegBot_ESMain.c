
// -- KegBot Main --
//
// Initializes Hardware and Starts the EventService Framework
//
// CREATED BY:
//  Adam Korycki
//  Donald Shannon
//  Robert Gaines
//  Rodger Wang

// mplab includes
#include <xc.h>
#include <stdio.h>

// framework includes
#include "ES_Configure.h"
#include "ES_Framework.h"

// uno32 includes
#include "BOARD.h"
#include "pwm.h"
#include "IO_Ports.h"
#include "timers.h"
// KegBot hardware includes
#include "MotorControl.h"
#include "tapeSensors.h"
#include "GlobalMacros.h"
#include "bumpers.h"
#include "TankCircuit.h"

void main(void)
{
    ES_Return_t ErrorType;

    BOARD_Init();
    TIMERS_Init();
    ES_Timer_Init();
    printf("Welcome to KegBot\r\n");

    // TODO add hardware initialization function calls
    printf("pwm init...\r\n");
    PWM_Init();
    PWM_SetFrequency(FREQ_1KHZ);
    printf("ad init...\r\n");
    AD_Init();
    AD_AddPins(BEACON_DETECTOR_PIN);
    printf("motor init...\r\n");
    Motor_Init();
    printf("bumper init...\r\n");
    Bumper_Init();
    printf("tape sensor init...\r\n");
    TS_Init();
    printf("Tank init...\r\n");
    Tank_Init();
    
    printf("Hardware Initialization Complete!\r\n");

    // now initialize the Events and Services Framework and start it running
    printf("Starting ES Framework\r\n");
    printf("using the 2nd Generation Events & Services Framework\r\n");
    ErrorType = ES_Initialize();
    if (ErrorType == Success) {
        ErrorType = ES_Run();

    }
    
    //loadNextBall();
    //if we got to here, there was an error
    switch (ErrorType) {
    case FailedPointer:
        printf("Failed on NULL pointer");
        break;
    case FailedInit:
        printf("Failed Initialization");
        break;
    default:
        printf("Other Failure: %d", ErrorType);
        break;
    }
    for (;;)
        ;

};

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
