#include <utils.h>

bool PotentiometerInWait(actions a)
{
    return (a == WAIT);
}

bool StrongestBeaconSignalFound(int old_IR, int new_IR)
{
    // return (new_IR > old_IR);
    int beacon_threshold = 10;
    return (new_IR > beacon_threshold);
}

lines detectLine(int pin, int min_red = 420, int black_min = 740)
{
    // Return WHITE, BLACK or RED; sensed from the IR line sensor.
    // From floor to top of sensor where the wires connect: exactly 1 inch.
    // int min_red = 440;   // drop this to 420 during daylight
    // int black_min = 690; // drop this to 500 during daylight
    if (analogRead(pin) > black_min)
    {
        return BLACK;
    }
    else if (analogRead(pin) < min_red)
    {
        return WHITE;
    }
    else
    {
        return RED;
    }
}

lines calcLine(int val)
{
    // Return WHITE, BLACK or RED; sensed from the IR line sensor.
    // From floor to top of sensor where the wires connect: exactly 1 inch.
    int min_red = 405;   // drop this to 420 during daylight
    int black_min = 695; // drop this to 500 during daylight
    if (val > black_min)
    {
        return BLACK;
    }
    else if (val < min_red)
    {
        return WHITE;
    }
    else
    {
        return RED;
    }
}

String StringLineColor(int pin)
{
    if (detectLine(pin) == BLACK)
    {
        return "BLACK";
    }
    else if (detectLine(pin) == RED)
    {
        return "RED";
    }
    else if (detectLine(pin) == WHITE)
    {
        return "WHITE";
    }
}

void PrintLineColors(int pin_left, int pin_right)
{
    Serial.println(
        "Left_Line, Right_Line: " + String(analogRead(pin_left)) + " (" + StringLineColor(pin_left) + ") , " + String(analogRead(pin_right)) + " (" + StringLineColor(pin_right) + ")");
}

void PrintLineColors(int pin_left, int pin_right, int pin_third, Metro &timer)
{
    if (timer.check())
    {
        timer.reset();
        Serial.println(
            "Third_Line, Left_Line, Right_Line: " + String(analogRead(pin_third)) + " (" + StringLineColor(pin_third) + ") , " + String(analogRead(pin_left)) + " (" + StringLineColor(pin_left) + ") , " + String(analogRead(pin_right)) + " (" + StringLineColor(pin_right) + ")");
    }
}

void StartRotatingCoM(int def_speed, int offset_B = 0)
{
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

void StartRotatingCoMReverse(int def_speed, int offset_B = 0)
{
    // Left motor turns clockwise.
    // Right motor turns counter-clockwise.

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

void StopMotors()
{
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

void MotorPulse(void (*func)(int, int), int speed, int offset_B = 0, int millis = 50)
{
    // Rotate both motors at equal torque, for 200ms.
    Metro tempTimer(millis); // [ms]
    tempTimer.reset();
    while (!tempTimer.check())
    {
        // Serial.println("Pulsing!");
        func(speed, offset_B);
    }
    StopMotors();
}

void MotorPulse(void (*func)(), int speed, int offset_B = 0, int millis = 50)
{
    // Rotate both motors at equal torque, for 200ms.
    Metro tempTimer(millis); // [ms]
    tempTimer.reset();
    while (!tempTimer.check())
    {
        // Serial.println("Pulsing!");
        func();
    }
    StopMotors();
}

void MoveForward(int def_speed = 220, int offset_B = 0)
{
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

void MoveBackward(int def_speed = 220, int offset_B = 0)
{
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

void RotateBackward(int def_speed = 220, int offset_B = 0)
{
    // Rotate both motors at equal torque.
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

void TurnRight(int def_speed = 220, int offset_B = 0)
{
    // Serial.println("Going right...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = (def_speed + offset_B) * .6;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void TurnLeft(int def_speed = 220, int offset_B = 0)
{
    // Serial.println("Going left...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = (def_speed + offset_A) * .6;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void RotateLeft(int def_speed = 220, int offset_B = 0)
{
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

void RotateRight(int def_speed = 220, int offset_B = 0)
{
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

void PivotBackRight(int def_speed = 220, int offset_B = 0)
{
    // Serial.println("Going right...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void PivotLeft(int def_speed = 220, int offset_B = 0)
{
    // Serial.println("Going left...");
    int offset_A = 0;

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = def_speed + offset_A;
    speed_B = def_speed + offset_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
}

void followLineFWD(baskets B, int def_speed = 220, int margin = 0, int offset_B = 0)
{

    // Third line sensor
    if ((B == BAD) && (detectLine(lineThirdPin_in, 620, 700) == RED))
    {
        Serial.println("ThirdLine Activated");
        return RotateLeft(100, 10);
    }
    else if ((B == GOOD) && (detectLine(lineThirdPin_in, 620, 700) == RED) && (detectLine(lineLeftPin_in, 500, 700) == RED || detectLine(lineRightPin_in, 650, 763) == RED))
    {
        Serial.println("ThirdLine Ignored");
        // return RotateRight(100, 10);
        // return MoveForward(def_speed, offset_B);
        last_line_follower = 0;
        return MotorPulse(StartRotatingCoM, 255, -30, 15);
    }

    // If Right Sensor and Left Sensor are at White color then it will call forword function
    if ((detectLine(lineRightPin_in, 650, 763) != RED) && (detectLine(lineLeftPin_in, 500, 700) != RED))
    {
        if (last_line_follower == 0)
        {
            Serial.println("RotateLeft");
            return RotateLeft(100, 10);
        }
        else if (last_line_follower == 1)
        {
            Serial.println("RotateRight");
            return RotateRight(120, 10);
        }
        else
        {
            Serial.println("MoveForward");
            return MoveForward(def_speed, offset_B);
        }
    }

    // If Right Sensor is Red and Left Sensor is White then it will call turn Right function
    if ((detectLine(lineRightPin_in, 650, 763) == RED) && (detectLine(lineLeftPin_in, 500, 700) != RED))
    {
        Serial.println("RotateRight");
        last_line_follower = 1;
        return TurnRight(110, 10);
    }

    // If Right Sensor is White and Left Sensor is Red then it will call turn Left function
    if ((detectLine(lineRightPin_in, 650, 763) != RED) && (detectLine(lineLeftPin_in, 500, 700) == RED))
    {
        Serial.println("RotateLeft");
        last_line_follower = 0;
        return TurnLeft(90, 10);
    }

    // If Right Sensor and Left Sensor are at Red color then it will call Stop function
    if ((detectLine(lineRightPin_in, 650, 763) == RED) && (detectLine(lineLeftPin_in, 500, 700) == RED))
    {
        Serial.println("MoveForward");
        last_line_follower = 2;
        return MoveForward(def_speed, offset_B);
    }
}

void followRedLine(int def_speed = 220, int margin = 0, int offset_B = 0, String both_white_action = "left")
{
    /* Controller to follow red tape line. */

    // If Right Sensor and Left Sensor are at White color then it will call forword function
    if ((detectLine(lineRightPin_in, 650, 763) != RED) && (detectLine(lineLeftPin_in, 500, 700) != RED))
    {
        if (both_white_action == "left")
        {
            Serial.println("RotateLeft");
            return RotateLeft(100, +20);
        }
        else if (both_white_action == "right")
        {
            Serial.println("RotateRight");
            return RotateRight(def_speed - margin, offset_B);
        }
        else
        {
            Serial.println("MoveForward");
            return MoveForward(def_speed, offset_B);
        }
    }

    // If Right Sensor is Red and Left Sensor is White then it will call turn Right function
    if ((detectLine(lineRightPin_in, 650, 763) == RED) && (detectLine(lineLeftPin_in, 500, 700) != RED))
    {
        Serial.println("RotateRight");
        return RotateRight(def_speed - margin, offset_B);
    }

    // If Right Sensor is White and Left Sensor is Red then it will call turn Left function
    if ((detectLine(lineRightPin_in, 650, 763) != RED) && (detectLine(lineLeftPin_in, 500, 700) == RED))
    {
        Serial.println("RotateLeft");
        return RotateLeft(100, +20);
    }

    // If Right Sensor and Left Sensor are at Red color then it will call Stop function
    if ((detectLine(lineRightPin_in, 650, 763) == RED) && (detectLine(lineLeftPin_in, 500, 700) == RED))
    {
        Serial.println("MoveForward");
        return MoveForward(def_speed, offset_B);
    }
}

bool beaconStrongEnough(int beaconVal)
{
    int beacon_threshold = 75;
    return (beaconVal > beacon_threshold);
}

void rotateAlphaGood()
{ // Studio
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

void rotateAlphaBad()
{ // Studio
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

void rotateBetaGood()
{ // Basket
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

void rotateBetaBad()
{ // Basket
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

void RaiseGate()
{
    // Raise ball gate.
    int angle_degrees = 0;
    GateServo.write(angle_degrees);
}

void LowerGate()
{
    // Lower ball gate.
    int angle_degrees = 90;
    GateServo.write(angle_degrees);
}

void FlickerGate()
{
    // Lower ball gate.
    GateServo.write(90);
    delay(235);
    GateServo.write(0);
}

void StopGate()
{
    // TODO.
    // Stop ball gate.
}

void WaveUpAtAudience()
{
    // TODO.
    // Raise arm to wave at audiance.
}

void WaveDownAtAudience()
{
    // TODO.
    // Lower arm to wave at audiance.
}

void StopWave()
{
    // TODO.
    // Stop arm that waves.
}