
#ifndef _GLOBALMACROS_H_
#define _GLOBALMACROS_H_

#include "pwm.h"
#include "IO_Ports.h"
#include "AD.h"

#define TRUE 1
#define FALSE 0


#define KEGBOT_DEBUG TRUE
//#define KEGBOT_DEBUG FALSE

#define FREQ_1KHZ 1000

#define GIVE_UP_ON_TOWER_TIME 20 * 1000 // 20 secs

// -- motor stuff --

#define MOTORSPEED 60
#define MOTORSPEEDOFFSET 12
#define ZERO 0



#define FORWARD 1
#define REVERSE 0

#define TRACKWIRE_PIN AD_PORTW7
#define TRACKWIRE_UPPER_LIM 890
#define TRACKWIRE_LOWER_LIM 850

// can be used to limit the max speed for the motors if there are problems
#define MAX_MOTOR_PWM 900

// MAKE SURE PINS DONT CONFLICT WITH OTHER IO STUFF
#define LEFT_MOTOR_PWM_PIN PWM_PORTY12
#define RIGHT_MOTOR_PWM_PIN PWM_PORTY10

#define HBRIDGE_CONTROL_PORT PORTX

#define HBRIDGE_LEFT_PINA PIN5
#define HBRIDGE_LEFT_PINB PIN6
#define HBRIDGE_RIGHT_PINA PIN3
#define HBRIDGE_RIGHT_PINB PIN4



/// -- beacon stuff --

#define BEACON_DETECTOR_PIN AD_PORTW8

#define BEACON_LOWER_THRESHOLD 300
#define BEACON_UPPER_THRESHOLD 800

// -- bumper pins --
// MAKE SURE PINS DONT CONFLICT WITH OTHER IO STUFF
#define BUMPER_CONTROL_PORT PORTX

#define FL_BUMPER_PIN PIN7
#define FR_BUMPER_PIN PIN8
#define RL_BUMPER_PIN PIN9
#define RR_BUMPER_PIN PIN10

// -- tape sensor stuff --
// TODO: find a good threshold
#define TS_THRESHOLD_UPPER 400
#define TS_THRESHOLD_LOWER 50
#define OFF_FACE_VALUE 900
#define PARALLEL_DIFFERENCE 0

#define TS_SETTLING_TIME 50 // in ms

#define TS_DEBOUNCE_TIME 100 // in ms

#define TS_FLOOR_FRONT 0
#define TS_FLOOR_LEFT 1
#define TS_FLOOR_REAR 2
#define TS_FLOOR_RIGHT 3
#define TS_SIDE_L 4
#define TS_SIDE_C 5
#define TS_SIDE_R 6
#define TS_NUM_SENSORS 7
#define TS_NUM_FLOOR_SENSORS 4

#define LED_ON 1
#define LED_OFF 0

#define ON_TAPE 1
#define OFF_TAPE 0

#define ALIGNED 1
#define NOT_ALIGNED 0

#define TS_NUM_EVENTS 5

// MAKE SURE PINS DONT CONFLICT WITH OTHER IO STUFF
#define TS_LEDS_CONTROL_PORT PORTX
#define TS_LEDS_PIN PIN11

#define TS_SIDE_L_PIN AD_PORTV3
#define TS_SIDE_C_PIN AD_PORTV4
#define TS_SIDE_R_PIN AD_PORTV5

#define TS_FLOOR_FRONT_PIN AD_PORTW3
#define TS_FLOOR_LEFT_PIN AD_PORTW4
#define TS_FLOOR_REAR_PIN AD_PORTW5
#define TS_FLOOR_RIGHT_PIN AD_PORTW6

//NAVIGATION DEFINES
#define REVERSE1_TICKS 1000
#define CCW_ADJUST_TICKS 1000
#define REVERSE_LEFT_TICKS 1000
#define TURN_RIGHT_TICKS 1000
#define SPIN_TICKS 1000

#define CW 1
#define CCW 0
// stepper magazine stuff
// stepper takes ports Y03 - Y08 inclusive
#define STEPS_FOR_FULL_ROTATION 210
#define DEGREES_PER_BALL 60
#define DEGREES_PER_ROTATION 360
#define STEPS_PER_BALL STEPS_FOR_FULL_ROTATION*DEGREES_PER_BALL/DEGREES_PER_ROTATION


// solenoid stuff
#define SOLENOID_PORT PORTV
#define SOLENOID_PIN PIN6

#define SOLENOID_OFF 0
#define SOLENOID_ON 1

#endif