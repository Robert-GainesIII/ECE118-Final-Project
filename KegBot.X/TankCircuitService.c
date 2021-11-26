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
#include "ES_Configure.h"
#include "BOARD.h"
#include "AD.h"

#include "ES_Framework.h"
#include "TankCircuitService.h"
#include "GlobalMacros.h"
#include "KegBot_TopHSM.h"
#include "TankCircuit.h"
#include "bumpers.h"
#include "timers.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

//#define BATTERY_DISCONNECT_THRESHOLD 175

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */

static uint8_t MyPriority;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitTankService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.

    // post the initial transition event
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostTankService(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunTankService(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
    //printf("ENTRY TO BUMPER SERVICE RUN");
    /********************************************
     in here you write your service code
     *******************************************/
    static ES_EventTyp_t lastEvent = ES_NO_EVENT;
    ES_EventTyp_t curEvent;
    
    ES_Event thisEvent;
    unsigned int tankVal = 0;
   
    
    switch (ThisEvent.EventType) {
        case ES_INIT:
            // No hardware initialization or single time setups, those
            // go in the init function above.
            //
            //printf("INIT TIMER");
            ES_Timer_InitTimer(TANK_TIMER, 100);
            // This section is used to reset service for some reason
            break;
        case ES_TIMERACTIVE:

            break;
        case ES_TIMERSTOPPED:

            break;

        case ES_TIMEOUT:
            
            curEvent = ES_NO_EVENT;
            
            ES_Timer_InitTimer(TANK_TIMER, 100);
            //printf("TIMEOUT");
            tankVal = TankRead();
            //printf("READ ADC TANK VALUE: %u \r\n", tankVal);
            
            if(tankVal == ERROR){
                curEvent = ES_ERROR;
            }else{
                if(tankVal > TRACKWIRE_UPPER_LIM){
                   curEvent = TRACK_WIRE_FOUND; 
                }
                if((tankVal < TRACKWIRE_LOWER_LIM) & lastEvent == TRACK_WIRE_FOUND){
                    curEvent = TRACK_WIRE_LOST;
                }
            }
            

            
            if (curEvent != lastEvent) {
                thisEvent.EventType = curEvent;
                ReturnEvent = thisEvent;
                lastEvent = curEvent;
                if (thisEvent.EventType == TRACK_WIRE_FOUND) {
                    //printf("TRACK WIRE FOUND!");

                }
                if (thisEvent.EventType == TRACK_WIRE_LOST) {
                    //printf("TRACK WIRE LOST!");

                }
                PostTopHSM(ReturnEvent);
            }
            break;

    }

    return ReturnEvent;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/

