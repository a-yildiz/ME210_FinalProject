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

void followRedLine(){
    // TODO.
    // Controller to follow red tape line.
}

lines detectLine(){
    // TODO.
    // Return WHITE, BLACK or RED; sensed from the IR line sensor.
    return WHITE;
}

int getBeaconSignal(int beaconPin_in){
    // TODO. 
    // Read from the Beacon pin:
    // beacon_reading = ...
    return 0;
}

bool beaconStrongEnough(int beaconVal){
    // TODO.
    // Check what a strong beacon level is.
    return false;
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

void RaiseLever(){
    // TODO.
    // Raise ball lever.
}

void LowerLever(){
    // TODO.
    // Lower ball lever.
}

void StopLever(){
    // TODO.
    // Stop ball lever.
}

void WaveUpAtAudience(){
    // TODO.
    // Raise arm to wave at audiance.
}

void WaveDownAtAudience(){
    // TODO.
    // Lower arm to wave at audiance.
}

void StopWave(){
    // TODO.
    // Stop arm that waves.
}