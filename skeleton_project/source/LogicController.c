#include "LogicController.h"

ElevatorConditions* elevatorConditions;
int* elevatorConditionArray[12];

time_t g_door_open_time;

ElevatorState getCurrentState() { return elevatorConditions->currentState; }
MotorDirection getMotorDirection() { return elevatorConditions->motorDirection; }

void makeElevatorDataArray(int* dataArray, const ElevatorConditions* conditions) {

    int currentFloor = 1; //elevio_lastFloor();
    int orderFloor = 2; //getOrder(getMotorDirection(), currentFloor);
    ElevatorState currentState = conditions->currentState;

    dataArray[0] = currentFloor <  orderFloor;
    dataArray[1] = currentFloor >  orderFloor;
    dataArray[2] = currentFloor == orderFloor;
    dataArray[3] = conditions->doorTimer;
    dataArray[4] = conditions->activeObstruction;
    dataArray[5] = conditions->stopPressed;
    
    for (int i=0; i<ELEVATORSTATE_LENGTH; i++) {
        dataArray[6+i] = currentState == i;
        printf("Data Array[%d]: %d\n", 6+i, dataArray[6+i]);
    }
}


ElevatorState getNextState( int* dataArray ) {
    
    int rulesAcheived[MATRIX_COLUMNS];

    for (int i=0; i<MATRIX_COLUMNS; i++) {
        rulesAcheived[i] = 1;
    }

    // if the mask & conditon matrix does not match with a rule, that rule is excluded.
    for (int i=0; i<MATRIX_ROWS; i++){
        for (int j=0; j<MATRIX_COLUMNS; j++){
            int b = MASK_MATRIX[i][j] & CONDITON_MATRIX[i][j];

            if ( b != dataArray[i] ){
                rulesAcheived[i] = 0;
            }

        }
    }


    ElevatorState nextState = getCurrentState();

    for (int i=0; i<MATRIX_COLUMNS; i++) {
        printf("rulesAcheived[%d], %d\n", i, rulesAcheived[i]);
        if ( rulesAcheived[i] ) {
            nextState = (ElevatorState)i;
        }
    }

    return nextState;
}


void runElevator(ElevatorState currentState, int is_new_state) {

    switch (currentState)
    {
    case OnFloor:
        elevio_motorDirection(DIRN_STOP);
        clearFromList(elevio_lastFloor());
        break;
    
    case MovingUp:
        elevio_motorDirection(DIRN_UP);
        break;
    
    case MovingDown:
        elevio_motorDirection(DIRN_DOWN);
        break;

    case DoorOpen:
        elevio_motorDirection(DIRN_STOP);
        doorHandling(is_new_state);
        break;

    case Obstruction:
        elevio_motorDirection(DIRN_STOP);
        break;

    case Stop:
        elevio_motorDirection(DIRN_STOP);
        break;
    }
} 


void updateLogicController() {
    elevatorConditions->activeObstruction = elevio_obstruction();
    elevatorConditions->stopPressed = elevio_stopButton();

    makeElevatorDataArray(elevatorConditionArray, elevatorConditions);
    
    ElevatorState state = getNextState(elevatorConditionArray);

    int is_new_state = elevatorConditions->currentState != state;
    elevatorConditions->currentState = state;

    runElevator(state, is_new_state);


    printf("Data Array: ");
    for (int i=0; i<MATRIX_ROWS; i++){
        printf( "%d ", elevatorConditionArray[i]);
    }
    printf("\n");
}



void doorHandling(int state_enter) {
    time_t current_time = time(NULL);

    if (state_enter) {
        g_door_open_time = current_time;
        printf("Doors closed\n");
    }

    time_t elapsed_time = current_time - g_door_open_time;
    if (elapsed_time >= 3){
        elevatorConditions->doorTimer = 1;
        printf("Doors opned\n");
    } else {
        elevatorConditions->doorTimer = 0;
    }
}
   





void testLogic() {

    int dataArray[MATRIX_ROWS];
    int* p_dataArray = &dataArray;

    ElevatorConditions ev;
    ElevatorConditions* p_ev;

    p_ev->activeObstruction=0;
    p_ev->currentState=MovingUp;
    p_ev->doorTimer=0;
    p_ev->motorDirection=DIRN_UP;
    p_ev->stopPressed=0;

    makeElevatorDataArray(p_dataArray, p_ev);

    printf("Data Array: ");
    for (int i=0; i<MATRIX_ROWS; i++){
        printf( "%d ", dataArray[i]);
    }
    printf("\n");

    ElevatorState state = getNextState(elevatorConditionArray);

    printf("State: %d", state);

    int is_new_state = elevatorConditions->currentState != state;
    elevatorConditions->currentState = state;

    printf("Entering new state: %d", state);

    // Go up
    runElevator(state, is_new_state);

    // 1 sek
    nanosleep(&(struct timespec){0, 1000*1000*1000}, NULL);

    // Go down
    p_ev->currentState=MovingDown;
    runElevator(state, is_new_state);

    nanosleep(&(struct timespec){0, 1000*1000*1000}, NULL);

    // stop and wait
    p_ev->currentState=OnFloor;
    runElevator(OnFloor, 1);
    p_ev->currentState=DoorOpen;
    updateLogicController();

    nanosleep(&(struct timespec){0, 1000*1000*1000}, NULL);

    // wait with obstruction
    p_ev->activeObstruction = 1;
    updateLogicController();

    nanosleep(&(struct timespec){0, 1000*1000*1000}, NULL);

    // wait for door to close and continue
    p_ev->activeObstruction = 0;

    while (1) {
        updateLogicController();
    }

}