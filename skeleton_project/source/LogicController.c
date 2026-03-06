#include "LogicController.h"

/* 
    #### STATIC HELPER FUNCTIONS DECLERATION ####
*/
static void makeElevatorDataArray(int* dataArray, const ElevatorConditions* p_conditions);
static ElevatorState getNextState( int* dataArray );
static void runElevator(ElevatorState currentState, int is_new_state, ElevatorConditions* p_conditions);
static void doorHandling(int state_enter);



// tmp
void delay_ms(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

/*
    #### Static variables ####
*/
static ElevatorConditions elevatorConditions;
static ElevatorConditions* p_elevatorConditions = &elevatorConditions;
static int* elevatorConditionArray[12];

static time_t g_door_open_time;
static MotorDirection lastMotorDirection = DIRN_STOP;


/*
    #### Global functions ####
*/
void setupLogicController() {
    p_elevatorConditions->currentState=OnFloor;
    p_elevatorConditions->doorTimerFinnished=0;
    p_elevatorConditions->motorDirection=DIRN_STOP;
    p_elevatorConditions->stopPressed=0;
}


void updateLogicController() {
    p_elevatorConditions->activeObstruction = elevio_obstruction();
    p_elevatorConditions->stopPressed = elevio_stopButton();

    makeElevatorDataArray(elevatorConditionArray, p_elevatorConditions);
    
    ElevatorState state = getNextState(elevatorConditionArray);

    int is_new_state = p_elevatorConditions->currentState != state;
    p_elevatorConditions->currentState = state;

    if (is_new_state) {
        printf("Data Array: ");
        for (int i=0; i<MATRIX_ROWS; i++){
            printf( "%d ", elevatorConditionArray[i]);
        }
        printf("\n");

        printf("Next order: %d\n", getOrder(getMotorDirection(), elevio_lastFloor()));
        printf("Transitoning in to state: %d\n", state);

        // update motordirection of previous state
        lastMotorDirection = getMotorDirection();
    }

    runElevator(state, is_new_state, p_elevatorConditions);

    
}


// global helper functions
ElevatorState getCurrentState() { return p_elevatorConditions->currentState; }
MotorDirection getMotorDirection() { return p_elevatorConditions->motorDirection; }



/*
    #### static helper function defenitions ####
*/
static void makeElevatorDataArray(int* dataArray, const ElevatorConditions* p_conditions) {

    int currentFloor = elevio_floorSensor();
    int lastFloor = elevio_lastFloor();
    int orderFloor = getOrder(getMotorDirection(), currentFloor);
    ElevatorState currentState = p_conditions->currentState;

    // determine what floor is ordered compared to elevator position (above, below or same floor)
    if (orderFloor == -1) { // no orders

        dataArray[0] = 0;
        dataArray[1] = 0;
        dataArray[2] = 0;

    } else  if ( getMotorDirection() == DIRN_STOP && currentFloor == -1 && lastFloor == orderFloor ) { // stoped between floors and going to last visited floor
        
        dataArray[0] = lastMotorDirection == DIRN_DOWN;
        dataArray[1] = lastMotorDirection == DIRN_UP;
        dataArray[2] = 0;

    } else { // normal case

        dataArray[0] = lastFloor <  orderFloor;
        dataArray[1] = lastFloor >  orderFloor;
        dataArray[2] = currentFloor == orderFloor;
    }
    


    // fill inn data given from ElevatorConditions struckt
    dataArray[3] = p_conditions->doorTimerFinnished;
    dataArray[4] = p_conditions->activeObstruction;
    dataArray[5] = p_conditions->stopPressed;
    
    // fill in current state
    for (int i=0; i<ELEVATORSTATE_LENGTH; i++) {
        dataArray[6+i] = currentState == i;
        // printf("Data Array[%d]: %d\n", 6+i, dataArray[6+i]);
    }
}


static ElevatorState getNextState( int* dataArray ) {
    
    int rulesAcheived[MATRIX_COLUMNS];

    for (int i=0; i<MATRIX_COLUMNS; i++) {
        rulesAcheived[i] = 1;
    }

    // if the mask & conditon matrix does not match with a rule, that rule is excluded.
    for (int i=0; i<MATRIX_ROWS; i++){
        for (int j=0; j<MATRIX_COLUMNS; j++){
            if (i == 0 && j==0) {
                // printf("Mask: %d\nData Array[0] %d\n", MASK_MATRIX[i][j], dataArray[i]);
            }
            int b = MASK_MATRIX[i][j] & dataArray[i];

            if ( b != CONDITON_MATRIX[i][j]){
                rulesAcheived[j] = 0;
            }

            // printf("%d ", b);

        }
        // printf("\n");
    }


    ElevatorState nextState = getCurrentState();
    
    if ( rulesAcheived[0] ) { nextState = MovingUp; }
    if ( rulesAcheived[1] ) { nextState = MovingDown; }
    if ( rulesAcheived[2] ) { nextState = OnFloor; }
    if ( rulesAcheived[3] ) { nextState = OnFloor; }
    if ( rulesAcheived[4] ) { nextState = OnFloor; }
    if ( rulesAcheived[5] ) { nextState = Obstruction; }
    if ( rulesAcheived[6] ) { nextState = OnFloor; }
    if ( rulesAcheived[7] ) { nextState = DoorOpen; }
    if ( rulesAcheived[8] ) { nextState = Stop; }

    if (getCurrentState() != nextState) {
        printf("rulesAcheived: ");
        for (int i=0; i<MATRIX_COLUMNS; i++) {
            printf(" %d", rulesAcheived[i]);
        }
        printf("\n");
    }

    return nextState;
}


static void runElevator(ElevatorState currentState, int is_new_state, ElevatorConditions* p_conditions) { 

    switch (currentState)
    {
    case OnFloor:
        p_elevatorConditions->motorDirection = DIRN_STOP;
        break;
    
    case MovingUp:
        p_elevatorConditions->motorDirection = DIRN_UP;
        break;
    
    case MovingDown:
        p_elevatorConditions->motorDirection = DIRN_DOWN;
        break;

    case DoorOpen:
        clearFloorFromList(elevio_lastFloor());  // clear floor when door is opned not closed
        p_elevatorConditions->motorDirection = DIRN_STOP;
        doorHandling(is_new_state);
        break;

    case Obstruction:
        p_elevatorConditions->motorDirection = DIRN_STOP;
        break;

    case Stop:
        p_elevatorConditions->motorDirection = DIRN_STOP;
        for (int i=0; i<N_FLOORS; i++) { clearFloorFromList(i); }
        break;
    }

    elevio_motorDirection( p_elevatorConditions->motorDirection );
} 


static void doorHandling(int state_enter) {
    time_t current_time = time(NULL);

    if (state_enter) {
        g_door_open_time = current_time;
        printf("Doors opned\n");
    }

    time_t elapsed_time = current_time - g_door_open_time;
    if (elapsed_time >= 3){
        p_elevatorConditions->doorTimerFinnished = 1;
        printf("Doors closed\n");
    } else {
        p_elevatorConditions->doorTimerFinnished = 0;
    }
}