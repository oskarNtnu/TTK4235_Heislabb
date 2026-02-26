#include "LogicController.h"

void makeElevatorDataArray(int* dataArray, const ElevatorConditions* conditions) {

    int currentFloor = elevio_lastFloor();
    int orderFloor = getOrder(getMotorDirection());
    ElevatorState currentState = conditions->currentState;

    dataArray[0] = currentFloor <  orderFloor;
    dataArray[1] = currentFloor >  orderFloor;
    dataArray[2] = currentFloor == orderFloor;
    dataArray[3] = conditions->doorTimer;
    dataArray[4] = conditions->activeObstruction;
    dataArray[5] = conditions->stopPressed;
    
    for (int i=0; i<ELEVATORSTATE_LENGTH; i++) {
        dataArray[6+i] = currentState == i;
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
        if ( rulesAcheived[i] ) {
            nextState = (ElevatorState)i;
        }
    }

    return nextState;
}


void runElevator() {
    printf("wroom");
}

void updateLogicController() {
    elevatorConditions->activeObstruction = elevio_obstruction();
    elevatorConditions->stopPressed = elevio_stopButton();

    makeElevatorDataArray(elevatorConditionArray, elevatorConditions);
    getNextState(elevatorConditionArray);
    runElevator();
}



void testLogic() {

    int dataArray[MATRIX_COLUMNS];
    int* p_dataArray = &dataArray;

    ElevatorConditions ev;
    ElevatorConditions* p_ev;

    p_ev->activeObstruction=1;
    p_ev->currentState=1;
    p_ev->dir=1;
    p_ev->doorTimer=1;
    p_ev->motorDirection=1;
    p_ev->stopPressed=1;

    makeElevatorDataArray(p_dataArray, p_ev);

    for (int i=0; i<MATRIX_COLUMNS; i++){
        printf(dataArray[i]);
    }

}