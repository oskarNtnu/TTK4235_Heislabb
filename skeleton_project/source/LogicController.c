#include "LogicController.h"
#include "OrderHandler.h"
#include "driver/elevio.h"

/* 
    #### STATIC HELPER FUNCTIONS DECLERATION ####
*/
static void makeElevatorDataArray(int* dataArray, const ElevatorConditions* p_conditions);
static ElevatorState getNextState( int* dataArray );
static void runElevator(ElevatorState currentState, int is_new_state, ElevatorConditions* p_conditions);
static void doorHandling(int state_enter);



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

    elevio_floorIndicator(elevio_lastFloor());

    makeElevatorDataArray(elevatorConditionArray, p_elevatorConditions);
    
    ElevatorState state = getNextState(elevatorConditionArray);

    int is_new_state = p_elevatorConditions->currentState != state;

    if (is_new_state) {
        printf("Data Array: ");
        for (int i=0; i<MATRIX_ROWS; i++){
            printf( "%d ", elevatorConditionArray[i]);
        }
        printf("\n");

        printf("Next order: %d\n", getOrder(elevio_lastFloor()));
        printf("Transitoning in to state: %d\n", state);

        // update motordirection of previous state
        if (p_elevatorConditions->currentState != Stop) {
            lastMotorDirection = getMotorDirection();
        }

        
        if (p_elevatorConditions->currentState == Stop && state != Stop && elevio_floorSensor() != -1) {
            state = DoorOpen;
        }

        p_elevatorConditions->currentState = state;

        printf("\n\n");
    }

    runElevator(state, is_new_state, p_elevatorConditions);

    
}


// global helper functions
ElevatorState getCurrentState() { return p_elevatorConditions->currentState; }
MotorDirection getMotorDirection() { return p_elevatorConditions->motorDirection; }



/*
    #### static helper function defenitions ####
*/

static int orderFloor = 0; // the floor that is currently ordered, used to determine if the elevator is between floors and going to the last visited floor

static void makeElevatorDataArray(int* dataArray, const ElevatorConditions* p_conditions) {

    int isOnFloor = elevio_floorSensor() != -1;

    int currentFloor = elevio_floorSensor();
    int lastFloor = elevio_lastFloor();
    orderFloor = getOrder(lastFloor);
    ElevatorState currentState = p_conditions->currentState;
    
    printf("\n");

    printf("order list up: ");
    printList(orderUpList);

    printf("order list down: ");
    printList(orderDownList);

    printf("Order floor: %d\n", orderFloor);

    // determine what floor is ordered compared to elevator position (above, below or same floor)
    if (orderFloor == -1) { // no orders

        dataArray[0] = 0;
        dataArray[1] = 0;
        dataArray[2] = 0;

        printf("No orders\n");

    } else  if ( getMotorDirection() == DIRN_STOP && currentFloor == -1 && lastFloor == orderFloor ) { // stoped between floors and going to last visited floor
        
        dataArray[0] = lastMotorDirection == DIRN_DOWN;
        dataArray[1] = lastMotorDirection == DIRN_UP;
        dataArray[2] = 0;

        printf("lastMotorDirection: %d\n", lastMotorDirection);
        printf("Stoped Between Floors\n");

    } else { // normal case

        dataArray[0] = lastFloor <  orderFloor;
        dataArray[1] = lastFloor >  orderFloor;
        dataArray[2] = currentFloor == orderFloor;

        printf("Normal Case, is at ordered floor %d\n", currentFloor == orderFloor);
    }
    
    printf("\n");


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
    if ( rulesAcheived[2] ) { nextState = DoorOpen; }
    if ( rulesAcheived[3] ) { nextState = OnFloor; }
    if ( rulesAcheived[4] ) { nextState = OnFloor; }
    if ( rulesAcheived[5] ) { nextState = Obstruction; }
    if ( rulesAcheived[6] ) { nextState = OnFloor; }
    if ( rulesAcheived[7] ) { nextState = DoorOpen; }
    if ( rulesAcheived[8] ) { nextState = Stop; }
    if ( rulesAcheived[9] ) { nextState = OnFloor; }


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

    int stopLight = 0;

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
        if( elevio_stopButton() )  {
            stopLight = 1;
            for (int i=0; i<N_FLOORS; i++) { clearFloorFromList(i); }
        }
        break;

    case Obstruction:
        p_elevatorConditions->motorDirection = DIRN_STOP;
        if( elevio_stopButton() )  {
            stopLight = 1;
            for (int i=0; i<N_FLOORS; i++) { clearFloorFromList(i); }
        }
        break;

    case Stop:
        p_elevatorConditions->motorDirection = DIRN_STOP;
        for (int i=0; i<N_FLOORS; i++) { clearFloorFromList(i); }
        stopLight = 1;
        if (elevio_floorSensor() != -1) {
            doorHandling(1); // open door if stop is pressed while on floor
        }
        break;
    }

    elevio_stopLamp(stopLight);
    elevio_motorDirection( p_elevatorConditions->motorDirection );
} 


static void doorHandling(int state_enter) {
    time_t current_time = time(NULL);

    if (state_enter) {
        g_door_open_time = current_time;
        printf("Doors opned\n");
        elevio_doorOpenLamp(1);
    }

    time_t elapsed_time = current_time - g_door_open_time;
    if (elapsed_time >= 3){
        p_elevatorConditions->doorTimerFinnished = 1;
        printf("Doors closed\n");
        elevio_doorOpenLamp(0);
    } else {
        p_elevatorConditions->doorTimerFinnished = 0;
    }
}