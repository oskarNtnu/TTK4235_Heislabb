#pragma once
#include "driver/elevio.h"
#include <stdio.h>


extern int orderUpList[4];
extern int orderDownList[4];

int getOrder(MotorDirection dir, int currentFloor);

void addOrderToList(int currentFloor, MotorDirection dir, int floorOrder, ButtonType buttonType,  int* upList, int* downList );
void clearFloorFromList(int floor, int* upList, int* downList);

void updateOrderHandler();

// Test functinos
void printList(const int* list);