#include <utils.h>

bool PotentiometerInWait(actions a){
    return (a == WAIT);
}

bool StrongestBeaconSignalFound(int old_IR, int new_IR){
    // return (new_IR > old_IR);
    int beacon_threshold = 100;
    return (new_IR > beacon_threshold);
}

lines detectLine(int pin){
    // Return WHITE, BLACK or RED; sensed from the IR line sensor.
    // From floor to top of sensor where the wires connect: exactly 1 inch.
    int margin    = 20;
    int avg_white = 420;
    int avg_red   = 570;
    int avg_black = 750;
    if (analogRead(pin) > avg_black-margin) {return BLACK;}
    else if (analogRead(pin) > avg_red-margin) {return RED;}
    else {return WHITE;}
}

void StartRotatingCoM(){
    // Left motor turns clockwise.
    // Right motor turns counter-clockwise.
    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = default_speed + PWM_OFFSET_A;
    speed_B = default_speed + PWM_OFFSET_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void StopMotors(){
    // Stop both motors.
    digitalWrite(DIR_A_1, HIGH);
    digitalWrite(DIR_A_2, HIGH);
    digitalWrite(DIR_B_1, HIGH);
    digitalWrite(DIR_B_2, HIGH);
    speed_A = 255;
    speed_B = 255;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void MoveForward(){
    // Rotate both motors at equal torque.
    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = default_speed + PWM_OFFSET_A;
    speed_B = default_speed + PWM_OFFSET_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void MoveBackward(){
    // Rotate both motors at equal torque.
    digitalWrite(DIR_A_1, !FORWARD_A);
    digitalWrite(DIR_A_2, FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = default_speed + PWM_OFFSET_A;
    speed_B = default_speed + PWM_OFFSET_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void TurnRight(){
    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = default_speed + PWM_OFFSET_A;
    speed_B = default_speed + PWM_OFFSET_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void TurnLeft(){
    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = default_speed;
    analogWrite(PWM_A, speed_A);
    speed_B = default_speed;
    analogWrite(PWM_B, speed_B);
}

void followRedLine(){
    /* Controller to follow red tape line. */

    // If Right Sensor and Left Sensor are at White color then it will call forword function
    if((detectLine(lineRightPin_in)==WHITE) || (detectLine(lineLeftPin_in)==WHITE)){
        MoveForward();
    }

    // If Right Sensor is Red and Left Sensor is White then it will call turn Right function  
    if((detectLine(lineRightPin_in)==RED) || (detectLine(lineLeftPin_in)==WHITE)){
        TurnRight();
    } 

    // If Right Sensor is White and Left Sensor is Red then it will call turn Left function
    if((detectLine(lineRightPin_in)==WHITE) || (detectLine(lineLeftPin_in)==RED)){
        TurnLeft();
    }

    // If Right Sensor and Left Sensor are at Red color then it will call Stop function
    if((detectLine(lineRightPin_in)==RED) || (detectLine(lineLeftPin_in)==RED)){
        MoveForward();
    }
}

bool beaconStrongEnough(int beaconVal){
    int beacon_threshold = 100;
    return (beaconVal > beacon_threshold);
}

void rotateAlphaGood(){  // Studio
    // Rotate AlphaGood amount through motors.
    digitalWrite(DIR_A_1, !FORWARD_A);
    digitalWrite(DIR_A_2, FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = default_speed;
    speed_B = default_speed;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void rotateAlphaBad(){  // Studio
    // Rotate AlphaBad amount through motors.
    digitalWrite(DIR_A_1, !FORWARD_A);
    digitalWrite(DIR_A_2, FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = default_speed;
    speed_B = default_speed;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void rotateBetaGood(){  // Basket
    // Rotate BetaGood amount through motors.
    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = default_speed;
    analogWrite(PWM_A, speed_A);
    speed_B = default_speed;
    analogWrite(PWM_B, speed_B);
}

void rotateBetaBad(){  // Basket
    // Rotate ABetaBad amount through motors.
    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = default_speed;
    analogWrite(PWM_A, speed_A);
    speed_B = default_speed;
    analogWrite(PWM_B, speed_B);
}

void RaiseGate(){
    // Raise ball gate.
    int angle_degrees = 0;
    GateServo.write(angle_degrees);
}

void LowerGate(){
    // Lower ball gate.
    int angle_degrees = 90;
    GateServo.write(angle_degrees);
}

void StopGate(){
    // TODO.
    // Stop ball gate.
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