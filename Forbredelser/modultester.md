

# ElevatorController

## E.1
elevatorUp() kjører heisen oppover.

## E.2
elevatorDown() kjører heisen nedover.

## E.3
elevatorUp og elevatorDown sammtidig??

elevatorStop() sammen med elevatorUp()

setFloorIndicator() setter riktig lys



# LogicController

neste ordre i etasje over når state er OnFloor
* kaller elevatorUp() fra ElevatorController

state er OnFloor
* Døra er lukket ?
neste ordre i samme etasje
* åpne dørene


# OrderHandler

getOrder() skal ikke returnere en etasje som er lik den nylige passerte etasjen
* Dersom dirrecion er opp og noen trykker på en knapp rett etter heisen har passert etasje n skal getOrder() returnere en etasje høyere enn n

addOrderToList() legger til ny bestilling fra den forrige etasjen i korrekt liste 
* Dersom addOrderToList() får en bestilling fra kabinettet til den samme etasjen den nettop forlot skal bestillingen bli lagt til i opp listen dersom heisen er på vei net og motsatt 