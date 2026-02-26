#include "OrderHanlder.h"


int orderUpList[4];
int orderDownList[4];

/*Get the next order to complete
* Check if the dirrection is up or down, goes trough orderUpList or orderDownList and returns the closest value that is 1 - except the current floor
* @param dir
*   Enum that tells if the elevator is moving up or down
* @param currentFloor
*   The last active floor
*/
int getOrder(MotorDirection dir, int currentFloor){
    if(dir == DIRN_UP){
        // If currentfloor = N_FLOORS-1 wont the loop start -> never an order above the upper floor
        for(int f = currentFloor+1; f<N_FLOORS; f++ ){ // currentFloor+1 is because if the elevator moves up, it can't have a order that have just passed  
            if(orderUpList[f] == 1){
                return f;
            }
        }
    }
    if(dir = DIRN_DOWN){
        for(int f = currentFloor-1; f>N_FLOORS; f--){
            if(orderDownList[f] == 1){
                return f;
            }
        }
    }
}

/* Add a button order to one of the order lists
First check if the buttontype is BUTTON_CAB, then compare it with the current floor, if the order is above, place in up list etc. 
Else check if it is a order up or down, and set the value in the corresponding list to 1 
@param currentfloor
Last active floor
@param dir
The dirrection of the elevator - used if an order is placed in the last floor while it moves away
@param floorOrder
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
void clearFloorFromList(int floor, int* upList, int* downList){}

void updateOrderHandler(){
    for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                int btnPressed = elevio_callButton(f, b);
                printf("Button pressed: %d\n", btnPressed);
                if(btnPressed){ 
                    elevio_buttonLamp(f, b, btnPressed);
                }

            }
        }
}