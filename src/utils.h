#include <Arduino.h>

/* Constants */
enum baskets {GOOD, BAD};
enum actions {MOVE, WAIT};
enum states  {AtStudioDisoriented, 
              AtStudioOrienting,
              AtStudioOriented,
              ChooseBasket,
              HeadingToBadBasket,
              HeadingToGoodBasket,
              FollowingRedTapeToBasket,
              IgnoreRedTape,
              DumpingBalls,
              HeadingBackFromBadBasket,
              HeadingBackFromGoodBasket,
              FollowingRedTapeToStudio};


uint8_t COLOR_BLANK[3] =     {LOW, LOW, LOW};
uint8_t COLOR_RED[3] =       {HIGH, LOW, LOW};
uint8_t COLOR_GREEN[3] =     {LOW, HIGH, LOW};
uint8_t COLOR_BLUE[3] =      {LOW, LOW, HIGH};
uint8_t COLOR_YELLOW[3] =    {HIGH, HIGH, LOW};
uint8_t COLOR_CYAN[3] =      {LOW, HIGH, HIGH};
uint8_t COLOR_PURPLE[3] =    {HIGH, LOW, HIGH};
uint8_t COLOR_WHITE[3] =     {HIGH, HIGH, HIGH};

#define BAD_POT_CUTOFF     102  // [out of 1024]
#define GOOD_POT_CUTOFF    922  // [out of 1024]



/* Functions */

bool inRange(int item, int lower, int upper){
  return (item <= upper) && (item >= lower);
}

void PrintVar(String label, String var){
  Serial.println("Value of " + label + ": " + var);
}

void PrintVar(String label, int var){
  Serial.println("Value of " + label + ": " + var);
}

void PrintVar(String label, float var){
  Serial.println("Value of " + label + ": " + var);
}

char detectStudioID(int spstPin_in){
  if (!digitalRead(spstPin_in)) {return 'A';}
  else {return 'B';}
}

bool approx_equal(int val1, int val2){
  int epsilon = 3;
  return (abs(val1-val2) < epsilon);
}