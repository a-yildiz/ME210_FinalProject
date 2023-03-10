#include <Arduino.h>
#include <Servo.h>
#include <Metro.h>


/* Pins (Don't forget to set pinMode in setup) --> remaining pin: 9 */ 
int potPin_in = A0;
int beaconPin_in = A1;
int lineRightPin_in = A2;
int lineLeftPin_in = A3;

int RGB_RED_PIN = 8;
int RGB_GREEN_PIN = 12;
int RGB_BLUE_PIN = 13;

int LEDPin_out = 10;
int gateServoPin_pwm_out = 11;

/* Motor Pins and Params */
int DIR_B_1 = 2;
int PWM_B   = 3;
int DIR_B_2 = 4;
int DIR_A_1 = 5;
int PWM_A   = 6;
int DIR_A_2 = 7;

int speed_A = 0;
int speed_B = 0;

int PWM_OFFSET_A = 0;
int PWM_OFFSET_B = -30; // 0

int default_speed = 140;

bool FORWARD_A = true;
bool FORWARD_B = false;


/* Time Constants */

int TIME_Studio_to_GoodCorner = 2500;
int TIME_GoodCorner_Rotation = 730;
int TIME_Stop_At_GoodCorner = 1000;
int TIME_GoodCorner_to_Basket = 1000;


/* Variable Constants */


Servo GateServo;

enum lines   {WHITE, RED, BLACK};
enum baskets {GOOD, BAD};
enum actions {MOVE, WAIT};
enum states  {AtStudioNotOriented, 
              AtStudioOrienting,
              AtStudioOrientDone,
              ChooseBasket,
              HeadingToBadBasket,
              HeadingToGoodBasket,
              BlindlyForwardBadBasket,
              BlindlyForwardGoodBasket,
              FollowingRedTapeToBasket,
              IgnoreRedTapeToBasket,
              IgnoreRedTapeToStudio,
              DumpingBalls,
              HeadingBackFromBadBasket,
              HeadingBackFromGoodBasket,
              FollowingRedTapeToStudio,
              StopIndefinitely,
              StopBrieflyBeforeTurn,
              RotateLeftToFindRedTape,
              RotateLeftToFindRedTape2,
              MissionEnd,
              DebugLineSensors,
              HeadToVerticalRedTape,
              BlindlyToRedTape,
              Anil00_Init,
              Anil01_Fwd,
              Anil02_Rotate,
              Anil03_StopBeforeGood,
              Anil04_FwdToGood,
              PureRedLineFollowing};


uint8_t RGB_BLANK[3]  = {LOW, LOW, LOW};
uint8_t RGB_RED[3]    = {HIGH, LOW, LOW};
uint8_t RGB_GREEN[3]  = {LOW, HIGH, LOW};
uint8_t RGB_BLUE[3]   = {LOW, LOW, HIGH};
uint8_t RGB_YELLOW[3] = {HIGH, HIGH, LOW};
uint8_t RGB_CYAN[3]   = {LOW, HIGH, HIGH};
uint8_t RGB_PURPLE[3] = {HIGH, LOW, HIGH};
uint8_t RGB_WHITE[3]  = {HIGH, HIGH, HIGH};

#define GOOD_POT_CUTOFF     102  // [out of 1024]
#define BAD_POT_CUTOFF    740  // [out of 1024]



/* Functions */

bool inRange(int item, int lower, int upper){
  return (item <= upper) && (item >= lower);
}

void PrintVar(String label, String var, Metro& timer){
  if (timer.check()){
    timer.reset();
    Serial.println("Value of " + label + ": " + var);
  }
}

void PrintVar(String label, int var, Metro& timer){
  if (timer.check()){
    timer.reset();
    Serial.println("Value of " + label + ": " + String(var));
  }
}

void PrintVar(String label, float var, Metro& timer){
  if (timer.check()){
    timer.reset();
    Serial.println("Value of " + label + ": " + String(var));
  }
}

char detectStudioID(int spstPin_in){
  if (!digitalRead(spstPin_in)) {return 'A';}
  else {return 'B';}
}

bool approx_equal(int val1, int val2){
  int epsilon = 3;
  return (abs(val1-val2) < epsilon);
}