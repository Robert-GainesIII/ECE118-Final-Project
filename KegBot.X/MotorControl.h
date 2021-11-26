#ifndef _MOTORCONTROL_H    /* Guard against multiple inclusion */
#define _MOTORCONTROL_H


//includes


void Motor_Init(void);

void LeftMtrSpeed(int speed, int direction);

void RightMtrSpeed(int speed, int direction);

void BothMtrSpeed(int speed, int direction);

void StopMotors(void);

//speed1 is meant to be faster value, direction can be set to reverse turn
void Turn(int speed1, int speed2, int direction);

//direction is 0-> CW 1-> CCW
void Rotate(int speed, int direction);

// rotates to the next ball in the magazine
void loadNextBall(void);

// actuates the release solenoid
// takes in SOLENOID_ON or SOLENOID_OFF
void setSolenoid(int position);


#endif // END OF FILE