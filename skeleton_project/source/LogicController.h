#include "driver/elevio.h"
#include "OrderHanlder.h"

typedef enum {
    OnFloor, MovingUp, MovingDown, DoorOpen, Obstruction, Stop, ELEVATORSTATE_LENGTH
} ElevatorState;


typedef struct {
    MotorDirection dir;
    int doorTimer;
    int activeObstruction;
    int stopPressed;
    ElevatorState currentState;
    MotorDirection motorDirection;

} ElevatorConditions;



ElevatorConditions* elevatorConditions;
int* elevatorConditionArray[12];

ElevatorState getCurrentState() { return elevatorConditions->motorDirection; };
MotorDirection getMotorDirection() { return elevatorConditions->motorDirection; };

void makeElevatorDataArray(int* dataArray, const ElevatorConditions* conditions);
ElevatorState getNextState( int* dataArray );

void runElevator();

void updateLogicController();

void testLogic();


#define MATRIX_ROWS 12
#define MATRIX_COLUMNS 9

static const int MASK_MATRIX[MATRIX_ROWS][MATRIX_COLUMNS] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static const int CONDITON_MATRIX[MATRIX_ROWS][MATRIX_COLUMNS] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
};
