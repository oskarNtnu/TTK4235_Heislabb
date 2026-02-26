
#include "elivio.h"

enum ElevatorState {
    OnFloor, MovingUp, MovingDown, DoorOpen, Obstruction, Stop
};


typedef struct {
    MotorDirection dir;
    bool doorTimer;
    bool activeObstruction;
    bool stopPressed;
    ElevatorState currentState;

} ElevatorConditions;



ElevatorState elevatorState;
ElevatorConditions elevatorConditions;
int elevatorCondition[12];

void makeElevatorDataArray(int* dataArray, const ElevatorConditions* conditions);
ElevatorState getNextState( int* dataArray );

runElevator();

void updateLogicController();
