#include "driver/elevio.h"

int orderUpList[4];
int orderDownList[4];

int getOrder(MotorDirection dir) {return 2;};

void addOrderToList(int floor, ButtonType type,  int* upList, int* downList );
void clearFloorFromList(int floor, int* upList, int* downList);

void updateOrderHandler();