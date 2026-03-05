#include "OrderHandler.h"
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

int main(){
    elevio_init();
    for(int f = 0; f < N_FLOORS; f++){
            for(int b = 0; b < N_BUTTONS; b++){
                    elevio_buttonLamp(f, b, 1);
            }
        }

    
    int k = 0;
    while(1){

    nanosleep(&(struct timespec){0, 20*1000*1000}, NULL);
    updateOrderHandler();
    // sleep(2);
    k++;
    if(k > 2){
        printf("\nUp list: ");
        printList(orderUpList);
        printf("Down list: ");
        printList(orderDownList);
        k = 0;
        }
    }
}