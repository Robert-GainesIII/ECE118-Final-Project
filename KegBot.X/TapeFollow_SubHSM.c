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
#include "TapeFollow_SubHSM.h"
#include "GlobalMacros.h"
#include "MotorControl.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    RotateCCW,
    RotateCW,
    Reverse,
    SharpTurn,

} AtTowerState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"RotateCCW",
	"RotateCW",
	"Reverse",
	"SharpTurn",
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

static AtTowerState_t CurrentState = InitPSubState; // <- change name to match ENUM
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
uint8_t InitTapeFollowHSM(void)
{
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunTapeFollowHSM(INIT_EVENT);
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
ES_Event RunTapeFollowHSM(ES_Event ThisEvent)
{
    uint8_t makeTransition = FALSE; // use to flag transition
    AtTowerState_t nextState; // <- change type to correct enum

    static int traverseDirection = CW;
    static int tapesFound = 0;

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
    case InitPSubState: // If current state is initial Psedudo State
        if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
        {
            // this is where you would put any actions associated with the
            // transition from the initial pseudo-state into the actual
            // initial state

            // now put the machine into the actual initial state
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        break;


        //STATE AFTER REVERSING TO ADJUST FOR A CLOCKWISE TRAVERSE
        //ALSO USED AS THE STATE FOR AJUSTING IF SHARP TURN TAKES TOO LONG
        //TO RESULT IN A COLLISION
    case RotateCCW:

        switch (ThisEvent.EventType) {
        case ES_ENTRY:

            //
            ES_Timer_InitTimer(CCW_ADJUST_TIMER2, 600);

            //CCW ROTATE
            LeftMtrSpeed(60, REVERSE);
            RightMtrSpeed(60, FORWARD);

            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case TAPE_LEFT:

            //FRONT LEFT BUMP ON A COUNTER CLOCK WISE ROTATION MOST LIKELY
            //MEANS THE BOT WAS TOO CLOSE TO THE TOWER AND SHOULD REVERSE
            StopMotors();
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;

            break;
        case TAPE_RIGHT:

            //FRONT RIGHT BUMP ON A COUNTER CLOCK WISE ROTATION MOST LIKELY
            //MEANS THE BOT WAS TOO CLOSE TO THE TOWER AND SHOULD REVERSE
            StopMotors();
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case TAPE_FRONT:

            //START MANUAEVER AROUND TOWER BEFORE TIMEOUT
            StopMotors();
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;


        case ES_TIMEOUT:
            //TIMER THAT TRIGGERS AMOUNT OF TIME FOR CCW ADJUSTMENT
            if (ThisEvent.EventParam == CCW_ADJUST_TIMER2) {
                StopMotors();
                nextState = SharpTurn;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;



        default:
            break;
        }
        break;

        //CASE FOR TURNING AROUND CORNERS FOR CW TRAVERSE OR AN ADUSTMENT FOR 
        //CCW REVERSE TRAVERSING
    case RotateCW:
        printf("ROTATE CW");
        readIR(FALSE);
        switch (ThisEvent.EventType) {
        case ES_ENTRY:
            //HERE BASED ON THE CW FLAG A TIMER WILL BE INITILIZED FOR FORWARD
            //OR REVERSE TRAVERSING
            ES_Timer_InitTimer(CCW_ADJUST_TIMER, 500);

            LeftMtrSpeed(70, FORWARD);
            RightMtrSpeed(70, REVERSE);

            ThisEvent.EventType = ES_NO_EVENT;
            break;

            //                case FRONT_L_BUMP:
            //                    //IF CW ROTATING AND 
            //                    StopMotors();
            //                    nextState = Reverse;
            //                    makeTransition = TRUE;
            //                    ThisEvent.EventType = ES_NO_EVENT;
            //                  
            //                    break;
        case TAPE_LEFT:
            //CW AJUSTMENT WORKED AND BOT IS HEAD FIRST INTO WALL
            //REVERSE, ADJUST, AND CONTINUE SHARP TURNS
            StopMotors();
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case TAPE_FRONT:
            //CW AJUSTMENT WORKED AND BOT IS HEAD FIRST INTO WALL
            //REVERSE, ADJUST, AND CONTINUE SHARP TURNS
            StopMotors();
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case TAPE_RIGHT:
            //START MANUAEVER AROUND TOWER THIS HAPPENS LESS OFTEN THAN THE 
            //FRONT BUMP
            StopMotors();
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
//            break;
        case ES_TIMEOUT:
            //TIMED AJUSTMENT FOR CW TRAVERSING AROUND CORNERS, VERY CLEAN
            if (ThisEvent.EventParam == CCW_ADJUST_TIMER) {
                StopMotors();
                nextState = SharpTurn;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        }
        break;

        //COLLSIONS OR TAPE WILL RESULT IN SMALL REVERSE THE AJUSTMENT OR SWITCHED
        //TRAVERSING DIRECTIONS
    case Reverse:
        printf("Reverse");
        readIR(FALSE);
        switch (ThisEvent.EventType) {
        case ES_ENTRY:
            //Reverse
            ES_Timer_InitTimer(REVERSE_TIMER1, 900);

            LeftMtrSpeed(70, REVERSE);
            RightMtrSpeed(70, REVERSE);

            ThisEvent.EventType = ES_NO_EVENT;

            break;

        case ES_TIMEOUT:
            if (ThisEvent.EventParam == REVERSE_TIMER1) {
                StopMotors();
                
                nextState = RotateCW;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

            //                case BACK_R_BUMP:
            //                    
            //                    StopMotors();
            //                    nextState = SharpTurn;
            //                    makeTransition = TRUE;
            //                    ThisEvent.EventType = ES_NO_EVENT;
            //                    break;
        case ES_EXIT:
            break;
        }
        break;

    case SharpTurn:
        printf("sharpturn");
        readIR(TRUE);
        switch (ThisEvent.EventType) {
        case ES_ENTRY:

            ES_Timer_InitTimer(TURN_TIMER, 1000);


            LeftMtrSpeed(70, FORWARD);
            RightMtrSpeed(90, FORWARD);

            ThisEvent.EventType = ES_NO_EVENT;
            break;

        case ES_TIMEOUT:
            if (ThisEvent.EventParam == TURN_TIMER) {
                StopMotors();
                nextState = RotateCCW;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        case TAPE_FRONT:
            StopMotors(); //reverse or some shit
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;

            break;
        case TAPE_RIGHT:

            StopMotors();
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
        case TAPE_LEFT:

            StopMotors();
            nextState = Reverse;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
            break;
            //                case TAPE_FRONT:
            //                     StopMotors();
            //                        ThisEvent.EventType = ES_NO_EVENT;
            //                    break;
            //                case TAPE_REAR:
            //                    StopMotors();
            //                        ThisEvent.EventType = ES_NO_EVENT;
            //                    break;
            //                case TAPE_LEFT:
            //                    StopMotors();
            //                        ThisEvent.EventType = ES_NO_EVENT;
            //                    break;
            //                case TAPE_RIGHT:
            //                    StopMotors();
            //                        ThisEvent.EventType = ES_NO_EVENT;
            //                    break;

        }
        break;
    default:
        break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunTapeFollowHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunTapeFollowHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}
