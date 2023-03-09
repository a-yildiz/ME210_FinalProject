#include <utils.h>

bool PotentiometerInWait(actions a){
    return (a == WAIT);
}

bool StrongestBeaconSignalFound(int old_IR, int new_IR){
    // return (new_IR > old_IR);
    int beacon_threshold = 10;
    return (new_IR > beacon_threshold);
}

lines detectLine(int pin){
    // Return WHITE, BLACK or RED; sensed from the IR line sensor.
    // From floor to top of sensor where the wires connect: exactly 1 inch.
    int min_red   = 420;
    int black_min = 500;
    if (analogRead(pin) > black_min) {
        return BLACK;}
    else if (analogRead(pin) < min_red) {
        return WHITE;}
    else {
        return RED;}
}

String StringLineColor(int pin){
    if (detectLine(pin)==BLACK){
        return "BLACK";
    }
    else if (detectLine(pin)==RED){
        return "RED";
    }
    else if (detectLine(pin)==WHITE){
        return "WHITE";
    }
}

void PrintLineColors(int pin_left, int pin_right){
    Serial.println(
        "Left_Line, Right_Line: "
        +String(analogRead(pin_left))+" ("
        +StringLineColor(pin_left)+") , "
        +String(analogRead(pin_right))+" ("
        +StringLineColor(pin_right)+")"
        );
}


void StartRotatingCoM(int def_speed, int offset_B=0){
    // Left motor turns clockwise.
    // Right motor turns counter-clockwise.

    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
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

void MotorPulse(void (*func)(int, int), int speed, int offset_B=0, int millis=50){
    // Rotate both motors at equal torque, for 200ms.
    Metro tempTimer(millis); // [ms]
    tempTimer.reset();
    while (!tempTimer.check()){
        // Serial.println("Pulsing!");
        func(speed, offset_B);
    }
    StopMotors();
}

void MoveForward(int def_speed=220, int offset_B=0){
    // Rotate both motors at equal torque.
    // Serial.println("Going forward...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void MoveBackward(int def_speed=220, int offset_B=0){
    // Rotate both motors at equal torque.
    int offset_A = 0;

    digitalWrite(DIR_A_1, !FORWARD_A);
    digitalWrite(DIR_A_2, FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void TurnRight(int def_speed=220, int offset_B=0){
    // Serial.println("Going right...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void TurnLeft(int def_speed=220, int offset_B=0){
    // Serial.println("Going left...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void RotateLeft(int def_speed=220, int offset_B=0){
    // Serial.println("Rotating left...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, !FORWARD_A);
    digitalWrite(DIR_A_2, FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void RotateRight(int def_speed=220, int offset_B=0){
    // Serial.println("Rotating right...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void followRedLine(int def_speed=220, int margin=0, int offset_B=0, String both_white_action="left"){
    /* Controller to follow red tape line. */

    // If Right Sensor and Left Sensor are at White color then it will call forword function
    if((detectLine(lineRightPin_in)!=RED) && (detectLine(lineLeftPin_in)!=RED)){
        if (both_white_action=="left") {
            Serial.println("RotateLeft");
            return RotateLeft(100, +20);
        }
        else if (both_white_action=="right"){
            Serial.println("RotateRight");
            return RotateRight(def_speed-margin, offset_B);
        }
        else {
        Serial.println("MoveForward");
        return MoveForward(def_speed, offset_B);
        }
    }

    // If Right Sensor is Red and Left Sensor is White then it will call turn Right function  
    if((detectLine(lineRightPin_in)==RED) && (detectLine(lineLeftPin_in)!=RED)){
        Serial.println("RotateRight");
        return RotateRight(def_speed-margin, offset_B);
    } 

    // If Right Sensor is White and Left Sensor is Red then it will call turn Left function
    if((detectLine(lineRightPin_in)!=RED) && (detectLine(lineLeftPin_in)==RED)){
        Serial.println("RotateLeft");
        return RotateLeft(100, +20);
    }

    // If Right Sensor and Left Sensor are at Red color then it will call Stop function
    if((detectLine(lineRightPin_in)==RED) && (detectLine(lineLeftPin_in)==RED)){
        Serial.println("MoveForward");
        return MoveForward(def_speed, offset_B);
    }
}

bool beaconStrongEnough(int beaconVal){
    int beacon_threshold = 75;
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