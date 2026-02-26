#include "driver/elevio.h"

int orderUpList[4];
int orderDownList[4];

int getOrder(MotorDirection dir);

void addOrderToList(int currentFloor, int floorOrder, ButtonType buttonType,  int* upList, int* downList );
void clearFloorFromList(int floor, int* upList, int* downList);

void updateOrderHandler();