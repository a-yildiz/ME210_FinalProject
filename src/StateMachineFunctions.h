#include <utils.h>

bool PotentiometerInWait(actions a){
    return (a == WAIT);
}

bool StrongestBeaconSignalFound(int old_IR, int new_IR){
    // TODO: Should account for hysteris/noise when comparing.
    return (new_IR > old_IR);
}

void StartRotatingCoM(){
    // TODO.
    // Left motor turns clockwise.
    // Right motor turns counter-clockwise.
}

void StopMotors(){
    // TODO.
    // Stop both motors.
}

void MoveForward(){
    // TODO.
    // Rotate both motors at equal torque.
}

int getBeaconSignal(int beaconPin_in){
    // TODO. 
    // Read from the Beacon pin:
    // beacon_reading = ...
    return 0;
}

void rotateAlphaGood(){
    // TODO.
    // Rotate AlphaGood amount through motors.
    }

void rotateAlphaBad(){
    // TODO.
    // Rotate AlphaBad amount through motors.
}

void rotateBetaGood(){
    // TODO.
    // Rotate BetaGood amount through motors.
}

void rotateBetaBad(){
    // TODO.
    // Rotate ABetaBad amount through motors.
}
