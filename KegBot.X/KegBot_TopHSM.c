/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel Elkaim and Soja-Marie Morgens
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is another template file for the SubHSM's that is slightly differet, and
 * should be used for all of the subordinate state machines (flat or heirarchical)
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */
// THIS IS THE VERSION I WANT

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/
#include "ES_Events.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "KegBot_TopHSM.h"
#include "SearchForTower_SubHSM.h" //#include all sub state machines called
#include "Traversing_SubHSM.h"
#include "TapeFollow_SubHSM.h"
#include "bumpers.h"
#include "MotorControl.h"
#include "GlobalMacros.h"
#include <stdio.h>
#include "RotateAroundTower.h"
#include "timers.h"
/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
//Include any defines you need to do

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/


typedef enum {
    InitPState,
    Rotating,
    Searching,
    Traversing,
    Shooting,
    Jiggle,
    Adjusting,
    NextTower,
    CurveForTape,
    TapeFollow,
    FindTape,

} TopHSMState_t;

static const char *StateNames[] = {
	"InitPState",
	"Rotating",
	"Searching",
	"Traversing",
	"Shooting",
	"Jiggle",
	"Adjusting",
	"NextTower",
	"CurveForTape",
	"TapeFollow",
	"FindTape",
};


/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine
   Example: char RunAway(uint_8 seconds);*/
/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static TopHSMState_t CurrentState = InitPState; // <- change enum name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitTopHSM(uint8_t Priority)
{
    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = InitPState;
    // post the initial transition event
    if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostTopHSM(ES_Event ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunTopHSM(ES_Event ThisEvent)
{
    uint8_t makeTransition = FALSE; // use to flag transition
    TopHSMState_t nextState; // <- change type to correct enum

    static int TRACK_WIRE_ON = FALSE;
    static int CORRECT_TOWER_BUMP_COUNT = 0;
    static unsigned int frontTime = 0;
    static unsigned int backTime = 0;
    // 0=ccw rotate
    // 1=back blip
    // 2=cw rotate
    // 3=forward blip
    // pulls out when both are pressed

    //0 for forward 1 for reverse
    static int TAPEDIR = 0;
    static int JIG = 0;
    static int IN_FIELD = 0;
    static int BUMPCOUNTFORTAPE = 0;
    static int DIR = 0;
    static int FR_BUMP_ON = TRUE;
    static int RR_BUMP_ON = FALSE;

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
    case InitPState: // If current state is initial Pseudo State
        if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
        {
            // this is where you would put any actions associated with the
            // transition from the initial pseudo-state into the actual
            // initial state
            // Initialize all sub-state machines

            //ENACTS A HOLD ONTO THE BALL REVOLVER 
            loadNextBall();

            InitSearchingSubHSM();

            InitRotateAroundTowerSubHSM();
            InitTraversingHSM();

            // now put the machine into the actual initial state
            nextState = Rotating;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;

        }
        break;

    case Rotating:
        switch (ThisEvent.EventType) {
        case ES_ENTRY:
            LeftMtrSpeed(70, FORWARD);
            RightMtrSpeed(70, REVERSE);
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case IR_FOUND:
            nextState = Searching;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;

    case Searching: // in the first state

        ThisEvent = RunSearchingSubHSM(ThisEvent);
        switch (ThisEvent.EventType) {


        case FRONT_L_BUMP:

            nextState = Traversing;
            makeTransition = TRUE;

            //printf("FRONT LEFT BUMP!");
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case FRONT_R_BUMP:

            nextState = Traversing;
            makeTransition = TRUE;

            //printf("FRONT RIGHT BUMP!");
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case TAPE_FRONT:

            nextState = TapeFollow;
            makeTransition = TRUE;

            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case TAPE_REAR:

            nextState = TapeFollow;
            makeTransition = TRUE;

            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case TAPE_LEFT:

            nextState = TapeFollow;
            makeTransition = TRUE;

            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case TAPE_RIGHT:

            nextState = TapeFollow;
            makeTransition = TRUE;

            ThisEvent.EventType = ES_NO_EVENT;
            break;

        }
        break;


    case Traversing:

        //ES_Timer_InitTimer(GIVE_UP_ON_TOWER_TIMER, 10000);
        //MAKE BOT GO AROUND TOWER ONCE BEFORE CHECKING FOR TRACKWIRE
        if (ThisEvent.EventType == FRONT_R_BUMP || ThisEvent.EventType == FRONT_L_BUMP) {
            CORRECT_TOWER_BUMP_COUNT++;
        }

        ThisEvent = RunTraversingHSM(ThisEvent);
        //ThisEvent = RunAtTowerSubHSM(ThisEvent);
        switch (ThisEvent.EventType) {

        case ES_ENTRY:
            printf("SPAM");
            ES_Timer_InitTimer(GIVE_UP_ON_TOWER_TIMER, 30000);

            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case TRACK_WIRE_FOUND:

            if (CORRECT_TOWER_BUMP_COUNT >= 2) {


                StopMotors();
                nextState = Adjusting;
                makeTransition = TRUE;
                TRACK_WIRE_ON = TRUE;
                CORRECT_TOWER_BUMP_COUNT = 0;
            }

            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case ES_TIMEOUT:
            // 0 = right blip
            // 1 = left blip
            // pulls out when both are pressed
            if (ThisEvent.EventParam == GIVE_UP_ON_TOWER_TIMER) {

                StopMotors();
                nextState = NextTower;
                CORRECT_TOWER_BUMP_COUNT = 0;
                makeTransition = TRUE;

            }
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case ES_EXIT:
            InitTraversingHSM();
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }



        break;

    case Adjusting:
        //ThisEvent = RunAtFaceSubHSM(ThisEvent);
        switch (ThisEvent.EventType) {
        case ES_ENTRY:
            LeftMtrSpeed(100, FORWARD);
            RightMtrSpeed(0, FORWARD);
            ES_Timer_StopTimer(CCW_ADJUST_TIMER);
            ES_Timer_StopTimer(CCW_ADJUST_TIMER2);
            ES_Timer_InitTimer(FORWARD_TIME, 600);

            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case ES_TIMEOUT:
            // 0 = right blip
            // 1 = left blip
            // pulls out when both are pressed
            if (ThisEvent.EventParam == CCW_ADJUST_TIMER2) {

                StopMotors();
                nextState = FindTape;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;

            }
            //            if (ThisEvent.EventParam == CCW_ADJUST_TIMER) {
            //
            //                LeftMtrSpeed(90, FORWARD);
            //                RightMtrSpeed(60, FORWARD);
            //                ThisEvent.EventType = ES_NO_EVENT;
            //
            //            }
            if (ThisEvent.EventParam == FORWARD_TIME) {

                frontTime = TIMERS_GetTime();
                printf("FRONT BUMP\r\n");
                LeftMtrSpeed(60, REVERSE);
                RightMtrSpeed(0, FORWARD);
                ThisEvent.EventType = ES_NO_EVENT;

            }
            ThisEvent.EventType = ES_NO_EVENT;
            break;


        case FRONT_R_BUMP:

            FR_BUMP_ON = TRUE;
            ES_Timer_StopTimer(FORWARD_TIME);
            frontTime = TIMERS_GetTime();
            printf("FRONT BUMP\r\n");
            LeftMtrSpeed(60, REVERSE);
            RightMtrSpeed(0, FORWARD);


            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case BACK_R_BUMP:

            printf("BACK BUMP\r\n");
            ES_Timer_InitTimer(CCW_ADJUST_TIMER2, (TIMERS_GetTime() - frontTime) / 2);

            LeftMtrSpeed(60, FORWARD);
            RightMtrSpeed(0, REVERSE);

            ThisEvent.EventType = ES_NO_EVENT;
            break;




        }
        break;
    case Shooting:
        //ThisEvent = RunAtFaceSubHSM(ThisEvent);
        //ThisEvent = RunRotateAroundTowerHSM(ThisEvent);
        switch (ThisEvent.EventType) {
        case ES_ENTRY:

            ES_Timer_InitTimer(SHOOTTIME, 500);
            StopMotors();
            setSolenoid(SOLENOID_ON);
            ThisEvent.EventType = ES_NO_EVENT;
            break;


        case ES_TIMEOUT:
            if (ThisEvent.EventParam == SHOOTTIME) {
                StopMotors();
                ES_Timer_InitTimer(ROTATE_DELAY, 1000);
                setSolenoid(SOLENOID_OFF);
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventParam == ROTATE_DELAY) {

                nextState = Jiggle;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;

            }
            break;
        }
        break;

    case Jiggle:
        switch (ThisEvent.EventType) {
        case ES_ENTRY:
            if (FRBumperIsPressed() == FALSE) {
                JIG = 0;
                LeftMtrSpeed(90, FORWARD);
                RightMtrSpeed(60, REVERSE);
            } else {
                JIG = 1;
                LeftMtrSpeed(90, REVERSE);
                RightMtrSpeed(60, FORWARD);
            }
            ES_Timer_InitTimer(SPIN_TIMER, 200);
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == SPIN_TIMER) {
                if (JIG == 0) {
                    LeftMtrSpeed(90, REVERSE);
                    RightMtrSpeed(60, FORWARD);
                } else {
                    LeftMtrSpeed(90, FORWARD);
                    RightMtrSpeed(60, REVERSE);
                }
                ES_Timer_InitTimer(ROTATE_DELAY, 200);
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventParam == ROTATE_DELAY) {
                StopMotors();
                loadNextBall();
                nextState = NextTower;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;

            }
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        }


        break;

    case NextTower:
        //InitRotateAroundTowerSubHSM();
        ThisEvent = RunRotateAroundTowerHSM(ThisEvent);

        switch (ThisEvent.EventType) {
        case IR_FOUND:
            nextState = Searching;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case TAPE_REAR:
            nextState = CurveForTape;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case TAPE_LEFT:
            nextState = CurveForTape;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }

        break;

    case CurveForTape:

        switch (ThisEvent.EventType) {
        case ES_ENTRY:

            RightMtrSpeed(70, FORWARD);
            LeftMtrSpeed(0, FORWARD);
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case FRONT_R_BUMP:
            StopMotors();
            nextState = Traversing;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case FRONT_L_BUMP:
            StopMotors();
            nextState = Traversing;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case TAPE_FRONT:
            StopMotors();
            nextState = TapeFollow;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;

            //        case TAPE_RIGHT:
            //            nextState = TapeFollow;
            //            makeTransition = TRUE;
            //            ThisEvent.EventType = ES_NO_EVENT;
            //            break;
        }

        break;


    case FindTape:

        switch (ThisEvent.EventType) {
        case ES_ENTRY:


            //while(1);
            LeftMtrSpeed(64, FORWARD);
            RightMtrSpeed(64, FORWARD);
            // ES_Timer_InitTimer(GIVE_UP_ON_TOWER_TIMER, 300);
            ThisEvent.EventType = ES_NO_EVENT;

            break;

        case FRONT_R_BUMP:
            LeftMtrSpeed(80, REVERSE);
            RightMtrSpeed(80, FORWARD);
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case FRONT_R_BUMP_LOST:
            LeftMtrSpeed(64, FORWARD);
            RightMtrSpeed(66, FORWARD);

            ThisEvent.EventType = ES_NO_EVENT;

            break;

        case BACK_R_BUMP:
            BUMPCOUNTFORTAPE = 1;
            LeftMtrSpeed(80, FORWARD);
            RightMtrSpeed(80, REVERSE);
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case BACK_R_BUMP_LOST:
            LeftMtrSpeed(64, REVERSE);
            RightMtrSpeed(66, REVERSE);

            ThisEvent.EventType = ES_NO_EVENT;

            break;

        case OFF_TOWER_R:
            //ES_Timer_StopTimer(GIVE_UP_ON_TOWER_TIMER);
            
                TAPEDIR = 1;
                StopMotors();
                LeftMtrSpeed(68, REVERSE);
                RightMtrSpeed(60, REVERSE);
            
            ThisEvent.EventType = ES_NO_EVENT;

            break;
        case OFF_TOWER_L:
            if (TAPEDIR == 0) {
                TAPEDIR = 1;
                StopMotors();
                LeftMtrSpeed(68, REVERSE);
                RightMtrSpeed(60, REVERSE);
            } else {
                StopMotors();
                LeftMtrSpeed(68, FORWARD);
                RightMtrSpeed(60, FORWARD);
            }
            ThisEvent.EventType = ES_NO_EVENT;

            break;
        case TOWER_TAPE_C_ALIGN:

            if (BUMPCOUNTFORTAPE == 1) {
                StopMotors();
                BUMPCOUNTFORTAPE = 0;
                TAPEDIR = 0;
                nextState = Shooting;
                makeTransition = TRUE;

            }
            ThisEvent.EventType = ES_NO_EVENT;
            break;
            //        case ES_TIMEOUT:
            //            if (ThisEvent.EventParam == GIVE_UP_ON_TOWER_TIMER) {
            //                StopMotors();
            //                BUMPCOUNTFORTAPE = 0;
            //                nextState = Searching;
            //                makeTransition = TRUE;
            //                ThisEvent.EventType = ES_NO_EVENT;
            //            }
            //            break;


        }
        break;

    case TapeFollow:
        ThisEvent = RunTapeFollowHSM(ThisEvent);

        switch (ThisEvent.EventType) {
        case IR_FOUND:
            nextState = Searching;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case FRONT_L_BUMP:

            nextState = Traversing;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case FRONT_R_BUMP:

            nextState = Traversing;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        }
        break;


    default:
        printf("invalid state in top HSM\r\n");
        break;

    } // end switch on Current State


    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunTopHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunTopHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}

