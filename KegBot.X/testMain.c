
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
    //ES_Timer_Init();
    printf("Welcome to KegBot\r\n");

    // TODO add hardware initialization function calls
    printf("pwm init...\r\n");
    PWM_Init();
    PWM_SetFrequency(FREQ_1KHZ);
    printf("ad init...\r\n");
    //AD_Init();

    printf("motor init...\r\n");
    Motor_Init();
    
    
   
    
    printf("Hardware Initialization Complete!\r\nTaking Steps Left");
    setSolenoid(SOLENOID_ON);
    int i = 0;
    for(i = 0; i < 100000; i++){
        
    }
    setSolenoid(SOLENOID_OFF);
//    for(i = 0; i < 2000000; i++){
//        
//  
    for (int i = 0; i < 10; i++){
      loadNextBall(); 
      for (int j = 0; j < 2000000; j++){}
      setSolenoid(SOLENOID_ON);
        for(int k = 0; k < 100000; k++){

        }
        setSolenoid(SOLENOID_OFF);
    }
    
    //
    for (;;)
        ;

};

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
