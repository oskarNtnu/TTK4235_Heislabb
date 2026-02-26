#pragma once


#define N_FLOORS 4

/*
DIRN_DOWN
DIRN_STOP
DIRN_UP
*/
typedef enum { 
    DIRN_DOWN   = -1,
    DIRN_STOP   = 0,
    DIRN_UP     = 1
} MotorDirection;


#define N_BUTTONS 3

/*
BUTTON_HALL_UP
BUTTON_HALL_DOWN
BUTTON_CAB
*/
typedef enum { 
    BUTTON_HALL_UP      = 0,
    BUTTON_HALL_DOWN    = 1,
    BUTTON_CAB          = 2
} ButtonType;


void elevio_init(void);

void elevio_motorDirection(MotorDirection dirn);
void elevio_buttonLamp(int floor, ButtonType button, int value);
void elevio_floorIndicator(int floor);
void elevio_doorOpenLamp(int value);
void elevio_stopLamp(int value);

int elevio_callButton(int floor, ButtonType button);
int elevio_floorSensor(void);
int elevio_lastFloor(void);
int elevio_stopButton(void);
int elevio_obstruction(void);

