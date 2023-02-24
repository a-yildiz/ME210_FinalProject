#include <utils.h>

#define USE_TIMER_1     true
#include <TimerInterrupt.h>


/* Pins */
int potPin_in = A1;
int LEDPin_out = 3;

int RGB_RED_PIN = 7;
int RGB_GREEN_PIN = 8;
int RGB_BLUE_PIN = 9;



/* Constants */
enum states  {GOING, COMING, INSTUDIO, VERTICAL};
enum baskets {GOOD, BAD};
enum actions {MOVE, STOP};

uint8_t COLOR_BLANK[3] =     {LOW, LOW, LOW};
uint8_t COLOR_RED[3] =       {HIGH, LOW, LOW};
uint8_t COLOR_GREEN[3] =     {LOW, HIGH, LOW};
uint8_t COLOR_BLUE[3] =      {LOW, LOW, HIGH};
uint8_t COLOR_YELLOW[3] =    {HIGH, HIGH, LOW};
uint8_t COLOR_CYAN[3] =      {LOW, HIGH, HIGH};
uint8_t COLOR_PURPLE[3] =    {HIGH, LOW, HIGH};
uint8_t COLOR_WHITE[3] =     {HIGH, HIGH, HIGH};

#define STUDIO_A_LED_FREQ    10  // [Hz]
#define STUDIO_B_LED_FREQ    3  // [Hz]

#define BAD_POT_CUTOFF     102  // [out of 1024]
#define GOOD_POT_CUTOFF    922  // [out of 1024]




/* Initializations */
states State = INSTUDIO;
baskets Basket = GOOD;
actions Action = STOP;

char Studio_ID = 'A';
bool LED_state = HIGH;
int pot_reading = 512;  // [out of 1024]



/* Function Definitions */
void LEDTimerHandler();
void updateBasketandAction();
void setRGBcolor(uint8_t color[]);


void setup() {
  /* DEBUG */
  // Serial.begin(9600);
  // while(!Serial);
  // Serial.println("Hello, world!");

  /* Pins */
  pinMode(potPin_in, INPUT);
  pinMode(LEDPin_out, OUTPUT);

  /* Detect Studio A or B from IR sensor. */
  // TODO. Change `Studio_ID` variable.

  /* Set LED brightness w.r.t. potentiometer reading, freq defined by Studio ID. */
  ITimer1.init();
  if (Studio_ID=='A'){
    ITimer1.attachInterrupt(STUDIO_A_LED_FREQ, LEDTimerHandler);
  }
  else {
    ITimer1.attachInterrupt(STUDIO_B_LED_FREQ, LEDTimerHandler);
  }


}





void loop() {

}


void LEDTimerHandler(){
  updateBasketandAction();

  if (LED_state == HIGH){
    LED_state = LOW;
    analogWrite(LEDPin_out, pot_reading);
    // PrintVar("LED State", LED_state);
  }
  else {
    LED_state = HIGH;
    analogWrite(LEDPin_out, 0);
    // PrintVar("LED State", LED_state);
  }
}



void updateBasketandAction(){
  pot_reading = map(analogRead(potPin_in), 0, 1023, 0, 255);
  // PrintVar("Pot", pot_reading);
  if (inRange(pot_reading, GOOD_POT_CUTOFF, 1023)){
    Basket = GOOD;
    Action = MOVE;
  }
  else if (inRange(pot_reading, 512, GOOD_POT_CUTOFF)){
    Basket = GOOD;
    Action = STOP;
  }
  else if (inRange(pot_reading, BAD_POT_CUTOFF, 512)){
    Basket = BAD;
    Action = STOP;
  }
  else {
    Basket = BAD;
    Action = MOVE;
  }
}

void setRGBcolor(uint8_t color[]){
  digitalWrite(RGB_RED_PIN, color[0]);
  digitalWrite(RGB_GREEN_PIN, color[1]);
  digitalWrite(RGB_BLUE_PIN, color[2]);
}