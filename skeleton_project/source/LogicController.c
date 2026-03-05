#include "LogicController.h"

void delay_ms(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}



ElevatorConditions elevatorConditions;
ElevatorConditions* p_elevatorConditions = &elevatorConditions;
int* elevatorConditionArray[12];

time_t g_door_open_time;

void logicControllerSetup() {
    p_elevatorConditions->currentState=OnFloor;
    p_elevatorConditions->doorTimer=0;
    p_elevatorConditions->motorDirection=DIRN_STOP;
    p_elevatorConditions->stopPressed=0;
}



ElevatorState getCurrentState() { return p_elevatorConditions->currentState; }
MotorDirection getMotorDirection() { return p_elevatorConditions->motorDirection; }

void makeElevatorDataArray(int* dataArray, const ElevatorConditions* conditions) {

    int currentFloor = elevio_lastFloor();
    int orderFloor = getOrder(getMotorDirection(), currentFloor);
    ElevatorState currentState = conditions->currentState;

    printf("Order floor: %d\n", orderFloor);

    dataArray[0] = currentFloor <  orderFloor;
    dataArray[1] = currentFloor >  orderFloor;
    dataArray[2] = currentFloor == orderFloor;
    dataArray[3] = conditions->doorTimer;
    dataArray[4] = conditions->activeObstruction;
    dataArray[5] = conditions->stopPressed;
    
    if (orderFloor == -1) {
        dataArray[0] = 0;
        dataArray[1] = 0;
        dataArray[2] = 0;
    }

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
            if (i == 0 && j==0) {
                printf("Mask: %d\nData Array[0] %d\n", MASK_MATRIX[i][j], dataArray[i]);
            }
            int b = MASK_MATRIX[i][j] & dataArray[i];

            if ( b != CONDITON_MATRIX[i][j]){
                rulesAcheived[j] = 0;
            }

            printf("%d ", b);

        }
        printf("\n");
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

    for (int i=0; i<MATRIX_COLUMNS; i++) {
        printf("rulesAcheived[%d], %d\n", i, rulesAcheived[i]);
    }

    return nextState;
}


void runElevator(ElevatorState currentState, int is_new_state) {

    printf("Running elevator in state: %d\n", currentState);

    switch (currentState)
    {
    case OnFloor:
        elevio_motorDirection(DIRN_STOP);
        clearFloorFromList(elevio_lastFloor());
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
    p_elevatorConditions->activeObstruction = elevio_obstruction();
    p_elevatorConditions->stopPressed = elevio_stopButton();

    makeElevatorDataArray(elevatorConditionArray, p_elevatorConditions);
    
    ElevatorState state = getNextState(elevatorConditionArray);

    int is_new_state = p_elevatorConditions->currentState != state;
    p_elevatorConditions->currentState = state;

    runElevator(state, is_new_state);


    printf("Data Array: ");
    for (int i=0; i<MATRIX_ROWS; i++){
        printf( "%d ", elevatorConditionArray[i]);
    }
    printf("\n");

    delay_ms(100);
}



void doorHandling(int state_enter) {
    time_t current_time = time(NULL);

    if (state_enter) {
        g_door_open_time = current_time;
        printf("Doors closed\n");
    }

    time_t elapsed_time = current_time - g_door_open_time;
    if (elapsed_time >= 3){
        p_elevatorConditions->doorTimer = 1;
        printf("Doors opned\n");
    } else {
        p_elevatorConditions->doorTimer = 0;
    }
}
   





void testLogic() {

    int dataArray[MATRIX_ROWS];
    int* p_dataArray = &dataArray;

    ElevatorConditions* p_ev = &elevatorConditions;

    p_ev->activeObstruction=0;
    p_ev->currentState=OnFloor;
    p_ev->doorTimer=0;
    p_ev->motorDirection=DIRN_UP;
    p_ev->stopPressed=0;

    makeElevatorDataArray(p_dataArray, p_ev);

    printf("Data Array: ");
    for (int i=0; i<MATRIX_ROWS; i++){
        printf( "%d ", dataArray[i]);
    }
    printf("\n");

    ElevatorState state = getNextState(p_dataArray);

    
    printf("\n\n");

    printf("State: %d\n", state);

    int is_new_state = p_ev->currentState != state;
    p_ev->currentState = state;

    printf("Entering new state: %d\n", state);

    
    // Go up
    runElevator(state, is_new_state);

    
    // 1 sek
    delay_ms(1000);

    printf("\n\n");

    // Go down
    p_ev->currentState=MovingDown;
    runElevator(MovingDown, is_new_state);
 
    delay_ms(1000);

    
    printf("\n\n");
    
    // stop and wait
    p_ev->currentState=OnFloor;
    runElevator(OnFloor, 1);
    p_ev->currentState=DoorOpen;
    updateLogicController();

    delay_ms(1000);

    printf("\n\n");
    // wait with obstruction
    p_ev->activeObstruction = 1;
    updateLogicController();

    delay_ms(1000);

    printf("\n\n");
    // wait for door to close and continue
    p_ev->activeObstruction = 0;

    while (1) {
        updateLogicController();
    }
    //*/
}