/*
 * File: TemplateService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "TapeSensorService.h"
#include <stdio.h>

#include "tapeSensors.h"
#include "GlobalMacros.h"

static uint8_t MyPriority;

//ADDED NEW EVENTS POST CREATION SO [5] [6] are for TOWER TAPE L AND R RESPECTIVELY
//[0] is for center
unsigned int currState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned int lastState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned int sensorVals[TS_NUM_SENSORS];

uint8_t InitTapeSensorService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

uint8_t PostTapeSensorService(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}



// -- TAPE SENSOR SERVICE --
// assumes the tape sensor hardware has already been initalized

// detects events for the following:
//      - wall tape sensors aligned with the right hole
//      - separate events for each floor tape sensor finding tape

// consecutive events are only pushed if 10 ms elapsed form the last event
// the voltage difference will be greater the lighter the surface is

// turns on LEDs
// 2 ms passes:
// samples light values for all sensors and turns off leds
// 2 ms passes:
// samples dark values for all sensors
// checks for a hole alignment event and only pushes if 10ms passed
// checks for each floor sensor event and only pushes if 10ms passed
// turns on the LEDs
#define TS_INIT 0
#define TS_LIGHT 1
#define TS_DARK 2

ES_Event RunTapeSensorService(ES_Event ThisEvent) {

    // only pushes an event if the current state is different than the last
    // inits to all 0s means it starts OFF_TAPE and NOT_ALIGNED
    switch (ThisEvent.EventType) {
        case ES_INIT:
            // No hardware initialization or single time setups, those
            // go in the init function above.
            //
            //printf("INIT TIMER");
            ES_Timer_InitTimer(TAPE_SENSOR_TIMER, 100);
            // This section is used to reset service for some reason
            break;
        case ES_TIMERACTIVE:

            break;
        case ES_TIMERSTOPPED:

            break;

        case ES_TIMEOUT:

            // for storing the light adc values for reference in the dark state
            if (ThisEvent.EventParam == TAPE_SENSOR_TIMER) {

                // the new event to push
                ES_Event newEvent;

                // reads the adc values
                // samples each sensor since 2ms should have passed for them to stabilize
                sensorVals[TS_SIDE_L] = AD_ReadADPin(TS_SIDE_L_PIN);
                //printf("%u -> sensor value\r\n", sensorVals[TS_SIDE_L]);
                sensorVals[TS_SIDE_C] = AD_ReadADPin(TS_SIDE_C_PIN);
                sensorVals[TS_SIDE_R] = AD_ReadADPin(TS_SIDE_R_PIN);
                sensorVals[TS_FLOOR_FRONT] = AD_ReadADPin(TS_FLOOR_FRONT_PIN);
                sensorVals[TS_FLOOR_LEFT] = AD_ReadADPin(TS_FLOOR_LEFT_PIN);
                sensorVals[TS_FLOOR_REAR] = AD_ReadADPin(TS_FLOOR_REAR_PIN);
                sensorVals[TS_FLOOR_RIGHT] = AD_ReadADPin(TS_FLOOR_RIGHT_PIN);
                //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                //printf("%u -> sensor value\r\n", sensorVals[TS_SIDE_L]);
                //printf("%u -> sensor value\r\n", sensorVals[TS_SIDE_L]);
                //printf("%u -> sensor value\r\n", sensorVals[TS_SIDE_L]);

                printf("%u %u %u -> sensor value\r\n", sensorVals[TS_SIDE_L], sensorVals[TS_SIDE_C], sensorVals[TS_SIDE_R]);
                
                // -- SIDE SENSORS --
                
                //EVENTS FOR CENTER TOWER TAPE SENSOR
                // pushes a the relevant events if the threshold is met
                if( (sensorVals[TS_SIDE_C] > TS_THRESHOLD_UPPER && sensorVals[TS_SIDE_C] < 600 ) &&  sensorVals[TS_SIDE_L] < 900 &&  sensorVals[TS_SIDE_R] < 900) { // on tape
                    currState[0] = TOWER_TAPE_C_ALIGN;
                } else{
                    currState[0] = ES_NO_EVENT;
                }
                // and pushes event if it wasnt on tape last time
                if (currState[0] != lastState[0]) {
                    // posts relevant event
                    lastState[0] = currState[0];
                    newEvent.EventType = currState[0];
                    PostTopHSM(newEvent);
                    //printf("ALIGMENT EVENT \r\n");
                }
//                else if ((currState[0] != lastState[0]) && (currState[0] == TOWER_TAPE_C_NOT_ALIGN)) {
//                    // posts relevant event
//                    newEvent.EventType = TOWER_TAPE_C_NOT_ALIGN;
//                    PostTopHSM(newEvent);
//                    lastState[0] = currState[0];
//                    //printf("ALIGMENT LOST EVENT\r\n");
//                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
//                }
//                if ((currState[0] != lastState[0]) && (currState[0] == OFF_TOWER_C)) {
//                    // posts relevant event
//                    newEvent.EventType = OFF_TOWER_C;
//                    PostTopHSM(newEvent);
//                    lastState[0] = currState[0];
//                    //printf("OFF TOWER EVENT\r\n");
//                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
//                }
////                
//                if(((sensorVals[TS_SIDE_C] - sensorVals[TS_SIDE_L] <= PARALLEL_DIFFERENCE) && (sensorVals[TS_SIDE_C] < 900) && (sensorVals[TS_SIDE_L] < 900)) || ((sensorVals[TS_SIDE_C] - sensorVals[TS_SIDE_R] <= PARALLEL_DIFFERENCE) && (sensorVals[TS_SIDE_C] < 900) && (sensorVals[TS_SIDE_R] < 900)) || ((sensorVals[TS_SIDE_L] - sensorVals[TS_SIDE_R] <= PARALLEL_DIFFERENCE) && (sensorVals[TS_SIDE_L] < 900) && (sensorVals[TS_SIDE_R] < 900) )){
//                    currState[7] = PARALLEL;
//                }else{
//                   currState[7] = NOT_PARALLEL; 
//                }
//                if (currState[7] != lastState[7] && currState[7] == PARALLEL) {
//                    // posts relevant event
//                    lastState[7] = currState[7];
//                    newEvent.EventType = PARALLEL;
//                    PostTopHSM(newEvent);
//                    printf("PARALLEL EVENT \r\n");
//                }
//                if (currState[7] != lastState[7] && currState[7] == NOT_PARALLEL) {
//                    // posts relevant event
//                    lastState[7] = currState[7];
//                    newEvent.EventType = NOT_PARALLEL;
//                    PostTopHSM(newEvent);
//                    //printf("NOT PARALLEL EVENT\r\n");
//                }

                
                
                //EVENTS FOR TOWER LEFT TAPE SENSOR
                // pushes a the relevant events if the threshold is met
                if (sensorVals[TS_SIDE_L] > TS_THRESHOLD_UPPER && sensorVals[TS_SIDE_L] < 750) { // on tape
                    currState[5] = TOWER_TAPE_L_ALIGN;
                } else if (sensorVals[TS_SIDE_L] < TS_THRESHOLD_LOWER) { // off tape
                    currState[5] = TOWER_TAPE_L_NOT_ALIGN;
                } else if (sensorVals[TS_SIDE_L] > OFF_FACE_VALUE){
                    currState[5] = OFF_TOWER_L;
                }
                // and pushes event if it wasnt on tape last time
                if (currState[5] != lastState[5] && currState[5] == TOWER_TAPE_L_ALIGN) {
                    // posts relevant event
                    lastState[5] = currState[5];
                    newEvent.EventType = TOWER_TAPE_L_ALIGN;
                    PostTopHSM(newEvent);
                    //printf("ALIGMENT EVENT LEFT");
                }
                else if ((currState[5] != lastState[5]) && (currState[5] == TOWER_TAPE_L_NOT_ALIGN)) {
                    // posts relevant event
                    newEvent.EventType = TOWER_TAPE_L_NOT_ALIGN;
                    PostTopHSM(newEvent);
                    lastState[5] = currState[5];
                    //printf("ALIGMENT LOST EVENT LEFT");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                if ((currState[5] != lastState[5]) && (currState[5] == OFF_TOWER_L)) {
                    // posts relevant event
                    newEvent.EventType = OFF_TOWER_L;
                    PostTopHSM(newEvent);
                    lastState[5] = currState[5];
                    //printf("OFF TOWER EVENT LEFT");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                // updates the last state


                //EVENTS FOR TOWER RIGHT TAPE SENSOR
                // pushes a the relevant events if the threshold is met
                if (sensorVals[TS_SIDE_R] > TS_THRESHOLD_UPPER && sensorVals[TS_SIDE_R] < 750) { // on tape
                    currState[6] = TOWER_TAPE_R_ALIGN;
                } else if (sensorVals[TS_SIDE_R] < TS_THRESHOLD_LOWER) { // off tape
                    currState[6] = TOWER_TAPE_R_NOT_ALIGN;
                } else if (sensorVals[TS_SIDE_R] > OFF_FACE_VALUE){
                    currState[6] = OFF_TOWER_R;
                }
                // and pushes event if it wasnt on tape last time
                if (currState[6] != lastState[6] && currState[6] == TOWER_TAPE_R_ALIGN) {
                    // posts relevant event
                    lastState[6] = currState[6];
                    newEvent.EventType = TOWER_TAPE_R_ALIGN;
                    PostTopHSM(newEvent);
                    //printf("ALIGMENT EVENT RIGHT");
                }
                else if ((currState[6] != lastState[6]) && (currState[6] == TOWER_TAPE_R_NOT_ALIGN)) {
                    // posts relevant event
                    newEvent.EventType = TOWER_TAPE_R_NOT_ALIGN;
                    PostTopHSM(newEvent);
                    lastState[6] = currState[6];
                    //printf("ALIGMENT LOST EVENT RIGHT");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                if ((currState[6] != lastState[6]) && (currState[6] == OFF_TOWER_R)) {
                    // posts relevant event
                    newEvent.EventType = OFF_TOWER_R;
                    PostTopHSM(newEvent);
                    lastState[6] = currState[6];
                    //printf("OFF TOWER EVENT RIGHT");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                // updates the last state
                
                
                
                
                
                
                
                
                // -- FRONT FLOOR --
                // checks if each floor tape sensor is on tape now


                if (sensorVals[TS_FLOOR_FRONT] > TS_THRESHOLD_UPPER && sensorVals[TS_FLOOR_FRONT] < 750) { // on tape
                    currState[1] = ON_TAPE;
                } else if (sensorVals[TS_FLOOR_FRONT] < TS_THRESHOLD_LOWER) { // off tape
                    currState[1] = OFF_TAPE;
                }
                // and pushes event if it wasnt on tape last time
                if (currState[1] != lastState[1] && currState[1] == ON_TAPE) {
                    // posts relevant event
                    lastState[1] = currState[1];
                    newEvent.EventType = TAPE_FRONT;
                    PostTopHSM(newEvent);
                    //printf("TAPE EVENT BOTTOM FRONT");
                }
                if ((currState[1] != lastState[1]) && (currState[1] == OFF_TAPE)) {
                    // posts relevant event
                    newEvent.EventType = TAPE_FRONT_OFF;
                    PostTopHSM(newEvent);
                    lastState[1] = currState[1];
                    //printf("TAPE EVENT BOTTOM FRONT OFF");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                // updates the last state





                // -- LEFT FLOOR --
                // checks if each floor tape sensor is on tape now



                if (sensorVals[TS_FLOOR_LEFT] > TS_THRESHOLD_UPPER && sensorVals[TS_FLOOR_LEFT] < 750) { // on tape
                    currState[2] = ON_TAPE;
                } else if (sensorVals[TS_FLOOR_LEFT] < TS_THRESHOLD_LOWER) { // off tape
                    currState[2] = OFF_TAPE;
                }
                // and pushes event if it wasnt on tape last time
                if (currState[2] != lastState[2] && currState[2] == ON_TAPE) {
                    // posts relevant event
                    lastState[2] = currState[2];
                    newEvent.EventType = TAPE_LEFT;
                    PostTopHSM(newEvent);
                    //printf("TAPE EVENT BOTTOM LEFT");
                }
                if ((currState[2] != lastState[2]) && (currState[2] == OFF_TAPE)) {
                    // posts relevant event
                    newEvent.EventType = TAPE_LEFT_OFF;
                    PostTopHSM(newEvent);
                    lastState[2] = currState[2];
                    //printf("TAPE EVENT LEFT OFF");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                //
                //    // updates the last state
                //    lastState[2] = currState[2];

                // -- RIGHT FLOOR --
                // checks if each floor tape sensor is on tape now
                if (sensorVals[TS_FLOOR_RIGHT] > TS_THRESHOLD_UPPER && sensorVals[TS_FLOOR_RIGHT] < 750) { // on tape
                    currState[3] = ON_TAPE;
                } else if (sensorVals[TS_FLOOR_RIGHT] < TS_THRESHOLD_LOWER && (lastState[3] == ON_TAPE)) { // off tape
                    //printf("OFF TAPE");
                    currState[3] = OFF_TAPE;
                }
                // and pushes event if it wasnt on tape last time
                if ((currState[3] != lastState[3]) && (currState[3] == ON_TAPE)) {
                    // posts relevant event
                    newEvent.EventType = TAPE_RIGHT;
                    lastState[3] = currState[3];
                    PostTopHSM(newEvent);
                    //printf("TAPE EVENT BOTTOM RIGHT");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                if ((currState[3] != lastState[3]) && (currState[3] == OFF_TAPE)) {
                    // posts relevant event
                    newEvent.EventType = TAPE_RIGHT_OFF;
                    PostTopHSM(newEvent);
                    lastState[3] = currState[3];
                    //printf("TAPE EVENT RIGHT OFF");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                // updates the last state


                // -- REAR FLOOR --
                // checks if each floor tape sensor is on tape now


                if (sensorVals[TS_FLOOR_REAR] > TS_THRESHOLD_UPPER && sensorVals[TS_FLOOR_REAR] < 750) { // on tape
                    currState[4] = ON_TAPE;
                } else if (sensorVals[TS_FLOOR_REAR] < TS_THRESHOLD_LOWER) { // off tape
                    currState[4] = OFF_TAPE;
                }
                // and pushes event if it wasnt on tape last time
                if (currState[4] != lastState[4] && currState[4] == ON_TAPE) {
                    // posts relevant event
                    lastState[4] = currState[4];
                    newEvent.EventType = TAPE_REAR;
                    PostTopHSM(newEvent);
                    //printf("TAPE EVENT BOTTOM REAR");
                }
                if ((currState[4] != lastState[4]) && (currState[4] == OFF_TAPE)) {
                    // posts relevant event
                    newEvent.EventType = TAPE_REAR_OFF;
                    PostTopHSM(newEvent);
                    lastState[4] = currState[4];
                    //printf("TAPE EVENT REAR OFF");
                    //printf("%u -> sensor value\r\n", sensorVals[TS_FLOOR_RIGHT]);
                }
                // updates the last state


                // starts the 2ms timer
                // needs 2ms for the sensors to stabilize after turning on and off the leds
                ES_Timer_InitTimer(TAPE_SENSOR_TIMER, TS_SETTLING_TIME);



                ES_Event ReturnEvent;
                ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
                return ReturnEvent;
            }
    }

}



// END OF FILE