#include <StateMachineFunctions.h>

#define USE_TIMER_1     true
#include <TimerInterrupt.h>

#include <Metro.h>


/* Pins (Don't forget to set pinMode in setup) */ 
int potPin_in = A1;
int beaconPin_in = A2;

int spstPin_in = 4;
int LEDPin_out = 3;

int RGB_RED_PIN = 7;
int RGB_GREEN_PIN = 8;
int RGB_BLUE_PIN = 9;



/* Initializations */
states State = AtStudioDisoriented;
baskets Basket = GOOD;
actions Action = WAIT;

char Studio_ID = 'A';
bool LED_state = HIGH;
int LED_freq = 10;   // [Hz]

int pot_reading = 512;  // [out of 1023]
int beacon_reading = 1023;

Metro PotTimer(1000);
Metro AlphaTimer(1000);


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
  pinMode(beaconPin_in, INPUT);

  pinMode(spstPin_in, INPUT);
  pinMode(LEDPin_out, OUTPUT);

  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);


  ITimer1.init();
  ITimer1.attachInterrupt(LED_freq, LEDTimerHandler);
}





void loop() {
  /* DEBUG */
  // setRGBcolor(COLOR_PURPLE);
  // delay(200);
  // setRGBcolor(COLOR_YELLOW);
  // delay(200);
  // setRGBcolor(COLOR_CYAN);
  // delay(200);

  /* Detect Studio A or B from IR sensor. */
  Studio_ID = detectStudioID(spstPin_in);

  /* Set LED brightness w.r.t. potentiometer reading. */
  ITimer1.setFrequency(LED_freq, LEDTimerHandler);
  
  /* State Machine */
  switch(State) {
    case AtStudioDisoriented:{
      if (PotentiometerInWait(Action)) {
        StartRotatingCoM();
        State = AtStudioOrienting;
      }
      break;
    }

    case AtStudioOrienting:{
      int new_beacon_reading = getBeaconSignal(beaconPin_in);
      if (StrongestBeaconSignalFound(beacon_reading, new_beacon_reading)) {
        StopMotors();
        State = AtStudioOriented;
        PotTimer.reset();
      }
      beacon_reading = new_beacon_reading;
      break;
    }

    case AtStudioOriented:{
      int new_pot_reading = analogRead(potPin_in);

      // Reset timer if the pot reading has changed.
      if (!approx_equal(pot_reading, new_pot_reading)) {PotTimer.reset();}

      // If 1 sec has passed since last pot reading change, then proceed to the chosen basket.
      if (PotTimer.check() && Action==MOVE){
        State = ChooseBasket;
        AlphaTimer.reset();
      }
      pot_reading = new_pot_reading;
      break;
    }

    case ChooseBasket:{
      if (Basket==GOOD){
        rotateAlphaGood();
        if (AlphaTimer.check()) {State = HeadingToGoodBasket;}  // Stop rotation after a duration.
      }
      else {
        rotateAlphaBad();
        if (AlphaTimer.check()) {State = HeadingToBadBasket;}   // Stop rotation after a duration.
      }
      break;
    }

    case HeadingToBadBasket:{
      MoveForward();
      // code block
      break;
    }

    case HeadingToGoodBasket:{
      MoveForward();
      // code block
      break;
    }

    case FollowingRedTapeToBasket:{
      // code block
      break;
    }

    case IgnoreRedTape:{
      // code block
      break;
    }

    case DumpingBalls:{
      // code block
      break;
    }

    case HeadingBackFromBadBasket:{
      // code block
      break;
    }

    case HeadingBackFromGoodBasket:{
      // code block
      break;
    }

    case FollowingRedTapeToStudio:{
        // code block
        break;
      }



    default:
    /* do nothing */
      break;
  }

}


void LEDTimerHandler(){
  updateBasketandAction();

  if (LED_state == HIGH){
    LED_state = LOW;
    analogWrite(LEDPin_out, map(pot_reading, 0, 1023, 0, 255));
  }
  else {
    LED_state = HIGH;
    analogWrite(LEDPin_out, 0);
  }
}



void updateBasketandAction(){
  // pot_reading = map(analogRead(potPin_in), 0, 1023, 0, 255);
  pot_reading = analogRead(potPin_in);  // [in range 0, 1023]
  // PrintVar("Pot", pot_reading);
  if (inRange(pot_reading, GOOD_POT_CUTOFF, 1023)){
    // Serial.println("C1");
    Basket = GOOD;
    Action = MOVE;
    LED_freq = 10; // [Hz]
  }
  else if (inRange(pot_reading, 512, GOOD_POT_CUTOFF)){
    // Serial.println("C2");
    Basket = GOOD;
    Action = WAIT;
    LED_freq = 50; // [Hz]
  }
  else if (inRange(pot_reading, BAD_POT_CUTOFF, 512)){
    // Serial.println("C3");
    Basket = BAD;
    Action = WAIT;
    LED_freq = 50; // [Hz]
  }
  else {
    // Serial.println("C4");
    Basket = BAD;
    Action = MOVE;
    LED_freq = 10; // [Hz]
  }
}

void setRGBcolor(uint8_t color[]){
  digitalWrite(RGB_RED_PIN, color[0]);
  digitalWrite(RGB_GREEN_PIN, color[1]);
  digitalWrite(RGB_BLUE_PIN, color[2]);
}

