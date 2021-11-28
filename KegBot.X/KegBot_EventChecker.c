/*
 * File:   TemplateEventChecker.c
 * Author: Gabriel Hugh Elkaim
 *
 * Template file to set up typical EventCheckers for the  Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the
 * names will have to be changed to match your code.
 *
 * This EventCheckers file will work with both FSM's and HSM's.
 *
 * Remember that EventCheckers should only return TRUE when an event has occured,
 * and that the event is a TRANSITION between two detectable differences. They
 * should also be atomic and run as fast as possible for good results.
 *
 * This file includes a test harness that will run the event detectors listed in the
 * ES_Configure file in the project, and will conditionally compile main if the macro
 * EVENTCHECKER_TEST is defined (either in the project or in the file). This will allow
 * you to check you event detectors in their own project, and then leave them untouched
 * for your project unless you need to alter their post functions.
 *
 * Created on September 27, 2013, 8:37 AM
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "KegBot_EventChecker.h"
#include "ES_Events.h"
#include "serial.h"
#include "AD.h"
//#include "tapeSensors.h"
#include "timers.h"
#include "GlobalMacros.h"
#include "KegBot_TopHSM.h"
#include "bumpers.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define BATTERY_DISCONNECT_THRESHOLD 175

/*******************************************************************************
 * EVENTCHECKER_TEST SPECIFIC CODE                                                             *
 ******************************************************************************/

//#define EVENTCHECKER_TEST
#ifdef EVENTCHECKER_TEST
#include <stdio.h>
#define SaveEvent(x) do {eventName=__func__; storedEvent=x;} while (0)

static const char *eventName;
static ES_Event storedEvent;
#endif

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this EventChecker. They should be functions
   relevant to the behavior of this particular event checker */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
static int FLBUFFER = 0;
static int FRBUFFER = 0;
static int RLBUFFER = 0;
static int RRBUFFER = 0;
uint8_t READIR = 1;
/* Any private module level variable that you might need for keeping track of
   events would be placed here. Private variables should be STATIC so that they
   are limited in scope to this module. */

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/


// -- BUMPER EVENT CHECKER -- 

uint8_t FLBumperEventChecker(void) {
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t currEvent;
    ES_Event thisEvent;
    int returnVal = FALSE;

    currEvent = ES_NO_EVENT;
    
    if(FLBumperIsPressed() == TRUE){
        FLBUFFER ++;
    }
    else{
        FLBUFFER  = ZERO;
    }
    
    if(FLBUFFER == ZERO & lastEvent == FRONT_L_BUMP){
        currEvent = FRONT_L_BUMP_LOST;
    }
    
    if(FLBUFFER >= 1000){
        currEvent = FRONT_L_BUMP;
        //printf("amount of bump clicks %i: \n\r", FLBUFFER);
    }
    
    
    if (currEvent != lastEvent) {
        thisEvent.EventType = currEvent;
        returnVal = TRUE;
        lastEvent = currEvent;
        //printf("BUMP! \n\r", FLBUFFER);
        PostTopHSM(thisEvent);
    }
    // do stuff

#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
    //PostTopHSM(thisEvent);
#else
    SaveEvent(thisEvent);
#endif   
    return (returnVal);
}

uint8_t FRBumperEventChecker(void) {
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t currEvent;
    ES_Event thisEvent;
    int returnVal = FALSE;
    currEvent = ES_NO_EVENT;
    
    if(FRBumperIsPressed() == TRUE){
        FRBUFFER ++;
    }
    else{
        FRBUFFER  = 0;
    }
    
    if(FRBUFFER == ZERO & lastEvent == FRONT_R_BUMP){
        currEvent = FRONT_R_BUMP_LOST;
    }
    
    
    if(FRBUFFER >= 1000){
        currEvent = FRONT_R_BUMP;
        //printf("amount of bump clicks %i: \n\r", FLBUFFER);
    }
   
    
    if (currEvent != lastEvent) {
        thisEvent.EventType = currEvent;
        returnVal = TRUE;
        lastEvent = currEvent;
        PostTopHSM(thisEvent);
        //printf("BUMP! \n\r", FRBUFFER);
    }
    // do stuff

#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
    //PostTopHSM(thisEvent);
#else
    SaveEvent(thisEvent);
#endif   
    return (returnVal);
}

uint8_t RLBumperEventChecker(void) {
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t currEvent;
    ES_Event thisEvent;
    int returnVal = FALSE;
    currEvent = ES_NO_EVENT;
    
   if(RLBumperIsPressed() == TRUE){
        RLBUFFER ++;
    }
    else{
        RLBUFFER  = 0;
    }
    
    if(RLBUFFER == ZERO & lastEvent == BACK_L_BUMP){
        currEvent = BACK_L_BUMP_LOST;
    }
    
    
    if(RLBUFFER >= 1000){
        currEvent = BACK_L_BUMP;
        //printf("amount of bump clicks %i: \n\r", FLBUFFER);
    }
   
    
    if (currEvent != lastEvent) {
        thisEvent.EventType = currEvent;
        returnVal = TRUE;
        lastEvent = currEvent;
        PostTopHSM(thisEvent);
        //printf("BUMP! \n\r", RLBUFFER);
    }

#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
    //PostTopHSM(thisEvent);
#else
    SaveEvent(thisEvent);
#endif   
    return (returnVal);
}

uint8_t RRBumperEventChecker(void) {
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t currEvent;
    ES_Event thisEvent;
    int returnVal = FALSE;
    currEvent = ES_NO_EVENT;
    
    if(RRBumperIsPressed() == TRUE){
        RRBUFFER ++;
    }
    else{
        RRBUFFER  = 0;
    }
    
    if(RRBUFFER == ZERO & lastEvent == BACK_R_BUMP){
        currEvent = BACK_R_BUMP_LOST;
    }
    
    
    if(RRBUFFER >= 1000){
        currEvent = BACK_R_BUMP;
        //printf("amount of bump clicks %i: \n\r", FLBUFFER);
    }
    
    
    if (currEvent != lastEvent) {
        thisEvent.EventType = currEvent;
        returnVal = TRUE;
        lastEvent = currEvent;
        PostTopHSM(thisEvent);
        //printf("BUMP! \n\r", RLBUFFER);
    }
    
    
    
    
    
    
    

#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
    //PostTopHSM(thisEvent);
#else
    SaveEvent(thisEvent);
#endif   
    return (returnVal);
}


uint8_t BeaconDetectionEventChecker(void) {
    //printf("beacon event checker\r\n");
    // stores the last state for tracking events
    if(READIR = 1){
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    // the hysteresis threshold
    static unsigned int threshold = BEACON_LOWER_THRESHOLD;
    // the current event type that may be pushed
    ES_EventTyp_t curEvent;
    // the event that will be pushed
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    uint16_t curr_lvl = AD_ReadADPin(BEACON_DETECTOR_PIN); // read the light level
    static int i = 0;
    //if (i++ % 1000 == 0){printf("Beacon Detector Value: %u \r\n", AD_ReadADPin(BEACON_DETECTOR_PIN));}

    // changes the threshold if a transition over the current threshold occurs
    // sets the relevant return event
    if (curr_lvl < threshold) {
        curEvent = IR_FOUND;
        threshold = BEACON_UPPER_THRESHOLD;
    } else {
   
        curEvent = IR_LOST;
        threshold = BEACON_LOWER_THRESHOLD;
        
    }
   
    // only pushes an event if there is a change from the last time
    // includes the actual ADC value just in case it may be relevant
    if (curEvent != lastEvent) {
        thisEvent.EventType = curEvent;
        thisEvent.EventParam = curr_lvl;
        returnVal = TRUE;
        lastEvent = curEvent;
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
    PostTopHSM(thisEvent);
#else
    SaveEvent(thisEvent);
#endif  
    }
   
    // updates the event history and returns
    
    return (returnVal);
    }
    else{
        return 0;
    }
}

void readIR(uint8_t flag){
    READIR = flag; 
}



/* 
 * The Test Harness for the event checkers is conditionally compiled using
 * the EVENTCHECKER_TEST macro (defined either in the file or at the project level).
 * No other main() can exist within the project.
 * 
 * It requires a valid ES_Configure.h file in the project with the correct events in 
 * the enum, and the correct list of event checkers in the EVENT_CHECK_LIST.
 * 
 * The test harness will run each of your event detectors identically to the way the
 * ES_Framework will call them, and if an event is detected it will print out the function
 * name, event, and event parameter. Use this to test your event checking code and
 * ensure that it is fully functional.
 * 
 * If you are locking up the output, most likely you are generating too many events.
 * Remember that events are detectable changes, not a state in itself.
 * 
 * Once you have fully tested your event checking code, you can leave it in its own
 * project and point to it from your other projects. If the EVENTCHECKER_TEST marco is
 * defined in the project, no changes are necessary for your event checkers to work
 * with your other projects.
 */
#ifdef EVENTCHECKER_TEST
#include <stdio.h>
static uint8_t(*EventList[])(void) = {EVENT_CHECK_LIST};

void PrintEvent(void);

void main(void) {
    BOARD_Init();
    /* user initialization code goes here */

    // Do not alter anything below this line
    int i;

    printf("\r\nEvent checking test harness for %s", __FILE__);

    while (1) {
        if (IsTransmitEmpty()) {
            for (i = 0; i< sizeof (EventList) >> 2; i++) {
                if (EventList[i]() == TRUE) {
                    PrintEvent();
                    break;
                }

            }
        }
    }
}

void PrintEvent(void) {
    printf("\r\nFunc: %s\tEvent: %s\tParam: 0x%X", eventName,
            EventNames[storedEvent.EventType], storedEvent.EventParam);
}
#endif