 #include "MotorControl.h"
#include <pwm.h>
#include <BOARD.h>
#include <xc.h>
#include "GlobalMacros.h"
#include "Stepper.h"
#include <IO_Ports.h>
#include <stdio.h>

// motor initalization
// assumes PWM has already been initialized

void Motor_Init(void) {
    
    if(Stepper_Init()== SUCCESS){
        printf("STEPPER INITILIZED SUCCESSUFLLY");
    }
    Stepper_SetRate(80);
    
    // Goes to enable on H bridge
    PWM_AddPins(LEFT_MOTOR_PWM_PIN);
    PWM_AddPins(RIGHT_MOTOR_PWM_PIN);

    // Goes into the direction inputs on the H bridge
    int pattern = 0x0;
    pattern |= HBRIDGE_LEFT_PINA;
    pattern |= HBRIDGE_LEFT_PINB;
    pattern |= HBRIDGE_RIGHT_PINA;
    pattern |= HBRIDGE_RIGHT_PINB;

    IO_PortsSetPortOutputs(HBRIDGE_CONTROL_PORT, pattern);

    // initializes all to LO
    IO_PortsClearPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_LEFT_PINA);
    IO_PortsClearPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_LEFT_PINB);
    IO_PortsClearPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_RIGHT_PINA);
    IO_PortsClearPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_RIGHT_PINB);
    

    // initializes the solenoid pins
    IO_PortsSetPortOutputs(SOLENOID_PORT, SOLENOID_PIN);
    IO_PortsClearPortBits(SOLENOID_PORT, SOLENOID_PIN);

}

// sets Left motor speed
// speed value between 0-100
// direction is FORWARD or REVERSE

void LeftMtrSpeed(int speed, int direction) {
    // limits speed
    
    //TO OFF SET DIFFERENCE IN MOTOR POWER
    speed = speed + 2;
    if (speed > 100) {
        speed = 100;
    }
    if (speed < 0) {
        speed = 0;
    }

    // sets new pwm
    int newPWM = speed * MAX_MOTOR_PWM / 100;
    PWM_SetDutyCycle(LEFT_MOTOR_PWM_PIN, newPWM);

    // sets direction
    if (direction == FORWARD) {
        // Hi - Lo
        IO_PortsClearPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_LEFT_PINA);
        IO_PortsSetPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_LEFT_PINB);
    } else {
        // Lo - Hi
        IO_PortsSetPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_LEFT_PINA);
        IO_PortsClearPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_LEFT_PINB);
        
    }
}

// sets right motor speed
// speed value between 0-100
// direction is FORWARD or REVERSE




void RightMtrSpeed(int speed, int direction) {
    // limits speed
//    if (direction == FORWARD) {
//        speed += MOTORSPEEDOFFSET;
//    } else {
//        speed -= MOTORSPEEDOFFSET;
//    }
    //speed = speed + 2;
    
    if (speed > 100) {
        speed = 100;
    }
    if (speed < 0) {
        speed = 0;
    }

    // sets new pwm
    int newPWM = speed * MAX_MOTOR_PWM / 100;
    PWM_SetDutyCycle(RIGHT_MOTOR_PWM_PIN, newPWM);

    // sets direction
    if (direction == FORWARD) {
        // Hi - Lo
        IO_PortsClearPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_RIGHT_PINA);
        IO_PortsSetPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_RIGHT_PINB);
    } else {
        // Lo - Hi
        IO_PortsSetPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_RIGHT_PINA);
        IO_PortsClearPortBits(HBRIDGE_CONTROL_PORT, HBRIDGE_RIGHT_PINB);
        
    }
    
}

// sets both motor speed
// speed value between 0-1000
// direction is FORWARD or REVERSE

void BothMtrSpeed(int speed, int direction) {
    RightMtrSpeed(speed, direction);
    LeftMtrSpeed(speed, direction);
}

void StopMotors(void) {
    RightMtrSpeed(ZERO, ZERO);
    LeftMtrSpeed(ZERO, ZERO);
}

void Turn(int speed1, int speed2, int direction) {
    if (direction == 0) {
        LeftMtrSpeed(speed2, direction);
        RightMtrSpeed(speed1, direction);
    } else {
        LeftMtrSpeed(speed1, direction);
        RightMtrSpeed(speed2, direction);
    }
}

void Rotate(int speed, int direction) {
    if (direction == 0) {
        LeftMtrSpeed(speed, direction);
        RightMtrSpeed(speed * -1, direction);
    } else {
        LeftMtrSpeed(speed * -1, direction);
        RightMtrSpeed(speed, direction);
    }
}


// rotates to the next ball in the magazine
void loadNextBall(void) {
    
    Stepper_InitSteps(FORWARD, 33);
    
    
}

// actuates the release solenoid
// takes in SOLENOID_ON or SOLENOID_OFF
void setSolenoid(int position) {
    if (position == SOLENOID_ON) {
        IO_PortsSetPortBits(SOLENOID_PORT, SOLENOID_PIN);
    } else {
        IO_PortsClearPortBits(SOLENOID_PORT, SOLENOID_PIN);
    }
}