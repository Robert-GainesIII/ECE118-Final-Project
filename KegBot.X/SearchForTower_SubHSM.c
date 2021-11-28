/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
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


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Events.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "KegBot_TopHSM.h"
#include "KegBot_EventChecker.h"
#include "SearchForTower_SubHSM.h"
#include "TapeFollow_SubHSM.h"
#include "MotorControl.h"
#include "GlobalMacros.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    CW_Rotate,
    Moving_To_Tower,
    SharpTurn,
    Reverse,
    CCW_Adjust,
    TapeFollow,

} SearchingState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"CW_Rotate",
	"Moving_To_Tower",
	"SharpTurn",
	"Reverse",
	"CCW_Adjust",
	"TapeFollow",
};

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static SearchingState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitSearchingSubHSM(void)
{
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunSearchingSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
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
ES_Event RunSearchingSubHSM(ES_Event ThisEvent)
{
    uint8_t makeTransition = FALSE; // use to flag transition
    SearchingState_t nextState; // <- change type to correct enum
    static int LOST_BEACON = 0;
    static int tape = 0;
    ES_Tattle(); // trace call stack

    switch (CurrentState) {
    case InitPSubState: // If current state is initial Psedudo State
        if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
        {
            // this is where you would put any actions associated with the
            // transition from the initial pseudo-state into the actual
            // initial state
            InitTapeFollowHSM();
            // now put the machine into the actual initial state
            nextState = Moving_To_Tower;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        break;

    case CW_Rotate: // in the first state, replace this with correct names

        //SET MOTORS TO EQUAL AND OPPOSITE VALUES TO ROTATE IN PLACE

        switch (ThisEvent.EventType) {
        case ES_ENTRY:
            readIR(TRUE);
            if (LOST_BEACON == 1) {
                ES_Timer_InitTimer(CCW_ADJUST_TIMER, 600);
            }
            LeftMtrSpeed(60, FORWARD);
            RightMtrSpeed(60, REVERSE);
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case IR_FOUND:
            //STOP MOTORS AS SOON AS EVENT IS DETECTED
            StopMotors();
            nextState = Moving_To_Tower;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case ES_TIMEOUT:
            if (ThisEvent.EventParam == CCW_ADJUST_TIMER) {
                //CHECKS ONLY TIMEOUT EVENT OF CORRECT TIMER

                //STOP MOTOR
                StopMotors();

                //TRANSITION
                nextState = CCW_Adjust;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
        case ES_NO_EVENT:
        default: // all unhandled events pass the event back up to the next level
            break;
        }
        break;

        //THIS CASE SHOULD ALSO HANDLE COLLISIONS MOST LIKELY
    case Moving_To_Tower: // in the first state, replace this with correct names
        switch (ThisEvent.EventType) {
        case ES_ENTRY:
            LOST_BEACON = 0;
            //RESET SEARCH FOR TOWER HSM FOR AFTER FIRST TOWER VISITED
            //DONT CONSUME THE COLLISION EVENT
            //TOP LEVEL HSM WILL USE THIS TO TRANSITION TO AT_TOWER
            //                    ES_Timer_InitTimer(READJUST_SEARCH_TIMER, 1000)
            if (tape == 0) {
                LeftMtrSpeed(65, FORWARD);
                RightMtrSpeed(65, FORWARD);
            }
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case IR_LOST:
            LOST_BEACON = 1;
            StopMotors();
            nextState = CW_Rotate;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;



        default: // all unhandled events pass the event back up to the next level
            break;
        }
        break;
    case SharpTurn: // in the first state, replace this with correct names
        switch (ThisEvent.EventType) {
        case ES_ENTRY:

            //START SHARP CW TURN
            LeftMtrSpeed(MOTORSPEED, FORWARD);
            RightMtrSpeed(MOTORSPEED / 2, FORWARD);
            ThisEvent.EventType = ES_NO_EVENT;
            break;
            //                case TAPE:
            //                    StopMotors();
            //                    nextState = Reverse;
            //                    makeTransition = TRUE;
            //                    ThisEvent.EventType = ES_NO_EVENT;
            //                    break;
        case IR_FOUND:
            //STOP MOTORS AS SOON AS BEACON IS DETECTED
            StopMotors();
            nextState = Moving_To_Tower;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case ES_NO_EVENT:
        default: // all unhandled events pass the event back up to the next level
            break;
        }
        break;
    case Reverse: // in the first state, replace this with correct names
        switch (ThisEvent.EventType) {
        case ES_ENTRY:
            ES_Timer_InitTimer(REVERSE_TIMER1, REVERSE1_TICKS);
            LeftMtrSpeed(MOTORSPEED, REVERSE);
            RightMtrSpeed(MOTORSPEED, REVERSE);
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == REVERSE_TIMER1) {
                //CHECKS ONLY TIMEOUT EVENT OF CORRECT TIMER

                //STOP MOTOR
                StopMotors();

                //TRANSITION
                nextState = CCW_Adjust;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        case ES_NO_EVENT:
        default: // all unhandled events pass the event back up to the next level
            break;
        }
        break;
    case CCW_Adjust: // in the first state, replace this with correct names
        switch (ThisEvent.EventType) {
        case ES_ENTRY:

            if (LOST_BEACON == 1) {
                ES_Timer_InitTimer(CCW_ADJUST_TIMER2, 1400);
            } else {
                ES_Timer_InitTimer(CCW_ADJUST_TIMER, CCW_ADJUST_TICKS);
            }
            //THINK ABOUT ADJUSTING MOTOR SPEED FOR THIS MANUEVER
            LeftMtrSpeed(60, REVERSE);
            RightMtrSpeed(60, FORWARD);
            break;
        case ES_TIMEOUT:
            if (ThisEvent.EventParam == CCW_ADJUST_TIMER) {
                //CHECKS ONLY TIMEOUT EVENT OF CORRECT TIMER

                StopMotors();

                //TRANSITION
                nextState = SharpTurn;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }

            if (ThisEvent.EventParam == CCW_ADJUST_TIMER2) {
                //CHECKS ONLY TIMEOUT EVENT OF CORRECT TIMER

                StopMotors();

                //TRANSITION

                //MAKE BOBBY EXPLAIN THIS
                LOST_BEACON = 0;
                nextState = CW_Rotate;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        case IR_FOUND:
            //STOP MOTORS AS SOON AS BEACON IS DETECTED
            StopMotors();
            nextState = Moving_To_Tower;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case ES_NO_EVENT:
        default: // all unhandled events pass the event back up to the next level
            break;
        }
        
        break;
    default: // all unhandled states fall into here
        break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunSearchingSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunSearchingSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

