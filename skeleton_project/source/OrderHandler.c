#include "OrderHandler.h"
#include "driver/elevio.h"


int orderUpList[4];
int orderDownList[4];
int currentOrder = -1;
MotorDirection preferedDirection = DIRN_UP;

/*Get the next order to complete
* Check if the dirrection is up or down, goes trough orderUpList or orderDownList and returns the closest value that is 1 - except the current floor
* @param dir
*   Enum that tells if the elevator is moving up or down
* @param currentFloor
*   The last active floor
*/
int updateOrder(int currentFloor) {
    if(preferedDirection == DIRN_UP){
        // If currentfloor = N_FLOORS-1 wont the loop start -> never an order above the upper floor
        for(int f = currentFloor+1; f<N_FLOORS; f++ ){ // currentFloor+1 is because if the elevator moves up, it can't have a order that have just passed  
            if(orderUpList[f] == 1){
                return f;
            }
        }
        // No orders above, switch prefered direction and check if there are orders below
        preferedDirection = DIRN_DOWN;
        for(int f = N_FLOORS-1; f>=0; f--){
            if(orderDownList[f] == 1){
                return f;
            }
        }

    }
    if(preferedDirection == DIRN_DOWN){
        for(int f = currentFloor-1; f>=0; f--){
            if(orderDownList[f] == 1){
                return f;
            }
        }
        // No orders below, switch prefered direction and check if there are orders above
        preferedDirection = DIRN_UP;
        if(preferedDirection == DIRN_UP){
        // If currentfloor = N_FLOORS-1 wont the loop start -> never an order above the upper floor
        for(int f = 0; f<N_FLOORS; f++ ){ // currentFloor+1 is because if the elevator moves up, it can't have a order that have just passed  
            if(orderUpList[f] == 1){
                return f;
            }
        }
    }}

    return -1; // No order in the direction of movement
}

int getOrder() {
    return currentOrder;
}

/* Add a button order to one of the order lists
First check if the buttontype is BUTTON_CAB, then compare it with the current floor, if the order is above, place in up list etc. 
Else check if it is a order up or down, and set the value in the corresponding list to 1 
@param currentfloor
Last active floor
@param dir
The dirrection of the elevator - used if an order is placed in the last floor while it moves away
@param floorOrder#include #
Floor wich a button places an order to
@param buttontype
Enum with the type of button, UP, Down, or cabin
@param upList
List with the orders upwards
@param downList
List with the orders downwards
*/
void addOrderToList(int currentFloor, MotorDirection dir, int floorOrder, ButtonType buttonType,  int* upList, int* downList ){
    if(buttonType == BUTTON_CAB){//Order from inside
        if(floorOrder>currentFloor){    upList[floorOrder] = 1;}
        else if (floorOrder<currentFloor){  downList[floorOrder] = 1;}
        // floor order == current floor
        else{
            if (dir == DIRN_DOWN){  upList[floorOrder] = 1;}
            else if(dir == DIRN_UP){    downList[floorOrder] = 1;}
            else {   upList[floorOrder] = 1; downList[floorOrder] = 1;} // if the elevator is stoped, the order can be placed in both lists, and the prefered direction will decide which one to complete first)
            // If the elevator is stopped, nothing should happen, order to the same floor
        }
    } 
    
    // Up Button from a floor
    else if(buttonType == BUTTON_HALL_UP){
        upList[floorOrder] = 1;
    }
    else if(buttonType == BUTTON_HALL_DOWN){
        downList[floorOrder] = 1;
    }

}
void clearFloorFromList(int floor){
    orderUpList[floor] = 0;
    orderDownList[floor] = 0;

    for (int b=0; b<N_BUTTONS; b++){
        elevio_buttonLamp(floor, b, 0);
    }
    currentOrder = updateOrder(floor);
}

void updateOrderHandler(){
    for(int f = 0; f < N_FLOORS; f++){
        for(int b = 0; b < N_BUTTONS; b++){
            int btnPressed = elevio_callButton(f, b);
            if(btnPressed){
                elevio_buttonLamp(f, b, 1);
                addOrderToList(elevio_lastFloor(), DIRN_STOP, f, b, orderUpList, orderDownList);
                currentOrder = updateOrder(elevio_lastFloor());
            }

        }
    }


}

void printList(const int* list){
    printf("Order list: [");
    for(int i=0; i<4; i++){
        printf("%d ", list[i]);
    }
    printf("]\n");
}

