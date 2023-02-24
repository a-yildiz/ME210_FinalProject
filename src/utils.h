#include <Arduino.h>


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
