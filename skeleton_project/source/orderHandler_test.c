#include "OrderHandler.h"
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

int main(){
    while(1){
    updateOrderHandler();
    nanosleep(&(struct timespec){0, 2*1000*1000*1000}, NULL);
    printf("Up list: ");
    printList(printList(orderUpList));
    printf("Down list: ");
    printList(printList(orderDownList));
    }
}