#include <StateMachineFunctions.h>

// #define USE_TIMER_2     true
// #include <TimerInterrupt.h>



/* Initializations */
states State = AtStudioDisoriented;
baskets Basket = GOOD;
actions Action = MOVE;

char Studio_ID = 'A';
bool LED_state = HIGH;
int LED_freq = 10;   // [Hz]
int no_of_waves = 0;

int pot_reading = 512;  // [out of 1023]
int beacon_reading = 1023;

int servo_angle = 0;

Metro PrintVarTimer(500);
Metro StateTimer(1000);
Metro WaveTimer(2000);
Metro MisssionTimer(130000);  // Mission time: 2m 10s = 130s = 130000ms


/* Function Definitions */
void ExecuteLEDandPotSM();
void ExecutePrimarySM();
void ExecutePowerSM();
void ExecuteRGBLightSM();
void ExecuteSafetySM();

// void LEDTimerHandler();
// void DummyHandler();
void setRGBcolor(uint8_t color[]);



void setup() {
  /* DEBUG */
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Hello, world!");

  /* Pins */
  pinMode(potPin_in, INPUT);
  pinMode(beaconPin_in, INPUT);
  pinMode(lineLeftPin_in, INPUT);
  pinMode(lineRightPin_in, INPUT);

  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  pinMode(LEDPin_out, OUTPUT);

  /* Servo Boot */
  GateServo.attach(gateServoPin_pwm_out);
  GateServo.write(0);

  /* Motor Boot */
  pinMode(DIR_A_1, OUTPUT);
  pinMode(DIR_A_2, OUTPUT);
  digitalWrite(DIR_A_1, LOW);
  digitalWrite(DIR_A_2, LOW);
  pinMode(PWM_A, OUTPUT);
  analogWrite(PWM_A, speed_A);
  pinMode(DIR_B_1, OUTPUT);
  pinMode(DIR_B_2, OUTPUT);
  digitalWrite(DIR_B_1, LOW);
  digitalWrite(DIR_B_2, LOW);
  pinMode(PWM_B, OUTPUT);
  analogWrite(PWM_B, speed_B);





  // ITimer2.init();
  // ITimer2.attachInterrupt(LED_freq, LEDTimerHandler);

  PrintVarTimer.reset();
  StateTimer.reset();
  WaveTimer.reset();
  MisssionTimer.reset();
  
}



void loop() {
  /* DEBUG */
  // PrintVar("State", State, PrintVarTimer);
  // PrintVar("Action", Action, PrintVarTimer);
  // delay(500);

  /* Detect Studio A or B from IR sensor. */
  // Studio_ID = detectStudioID(spstPin_in);

  // /* Set LED brightness w.r.t. potentiometer reading. */
  // // ITimer2.setFrequency(LED_freq, LEDTimerHandler);
  
  // RaiseGate();
  // LowerGate();
  
  // GateServo.write(180);
  // delay(500);
  // GateServo.write(60);
  // delay(500);
  // GateServo.write(90);
  // delay(500);
  // GateServo.write(-180);

  PrintVar("State", State, PrintVarTimer);
  PrintVar("Action", Action, PrintVarTimer);


  /* State Machines */
  ExecuteLEDandPotSM();
  ExecutePrimarySM();
  ExecuteRGBLightSM();
  // ExecutePowerSM();
  // ExecuteSafetySM();



  // int beacon_val =  analogRead(beaconPin_in);
  // PrintVar("beacon val", beacon_val, PrintVarTimer);

  // PrintVar("pot reading", pot_reading, PrintVarTimer);

  // delay(2000);
  // GateServo.write(0);
  // Serial.println(0);

  // delay(2000);
  // GateServo.write(60);
  // Serial.println(60);

  // delay(2000);
  // GateServo.write(90);
  // Serial.println(90);

  // delay(2000);
  // GateServo.write(-180);
  // Serial.println(-180);

}


void ExecutePrimarySM(){
  switch(State) {
    case AtStudioDisoriented:{
      if (PotentiometerInWait(Action)) {
        StartRotatingCoM();
        State = AtStudioOrienting;
      }
      break;
    }

    case AtStudioOrienting:{
      int new_beacon_reading = analogRead(beaconPin_in);
      if (beaconStrongEnough(new_beacon_reading)) {
        StopMotors();
        State = AtStudioOriented;
        StateTimer.reset();
      }
      beacon_reading = new_beacon_reading;
      break;
    }

    case AtStudioOriented:{
      int new_pot_reading = analogRead(potPin_in);

      // Reset timer if the pot reading has changed.
      if (!approx_equal(pot_reading, new_pot_reading)) {StateTimer.reset();}

      // If 1 sec has passed since last pot reading change, then proceed to the chosen basket.
      if (StateTimer.check() && Action==MOVE){
        State = ChooseBasket;
        StateTimer.reset();
      }
      pot_reading = new_pot_reading;
      break;
    }

    case ChooseBasket:{
      if (Basket==GOOD){
        rotateAlphaGood();
        if (StateTimer.check()) {State = HeadingToGoodBasket;}  // Stops rotation after a duration.
      }
      else {
        rotateAlphaBad();
        if (StateTimer.check()) {State = HeadingToBadBasket;}   // Stops rotation after a duration.
      }
      break;
    }

    case HeadingToBadBasket:{
      MoveForward();
      if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
        State = FollowingRedTapeToBasket;
      }
      break;
    }

    case HeadingToGoodBasket:{
      MoveForward();
      if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
        State = IgnoreRedTapeToBasket;
        StateTimer.reset();
      }
      break;
    }

    case FollowingRedTapeToBasket:{
      followRedLine();
      if ((detectLine(lineLeftPin_in)==BLACK) || (detectLine(lineRightPin_in)==BLACK)){
        State = DumpingBalls;
        StateTimer.reset();
      }
      break;
    }

    case IgnoreRedTapeToBasket:{
      if (StateTimer.check()){
        if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
          State = FollowingRedTapeToBasket;
        }
      }
      break;
    }

    case IgnoreRedTapeToStudio:{
      if (StateTimer.check()){
        if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
          State = FollowingRedTapeToStudio;
        }
      }
      break;
    }

    case DumpingBalls:{
      StopMotors();
      RaiseGate();

      if (StateTimer.check()){
        LowerGate();
        StateTimer.reset();

        if (Basket==BAD){
          rotateBetaBad();
          State = HeadingBackFromBadBasket;
        }
        else {
          rotateBetaGood();
          State = HeadingBackFromGoodBasket;
        }
      }
      break;
    }

    case HeadingBackFromBadBasket:{
      if (StateTimer.check()){
        MoveForward();
        if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
          followRedLine();
          State = FollowingRedTapeToStudio;
        }
      }
      break;
    }

    case HeadingBackFromGoodBasket:{
      if (StateTimer.check()){
        MoveForward();
        if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
          State = IgnoreRedTapeToStudio;
          StateTimer.reset();
        }
      }
      break;
    }

    case FollowingRedTapeToStudio:{
      int beacon_val = analogRead(beaconPin_in);
      if (beaconStrongEnough(beacon_val)){
        StopMotors();
        State = AtStudioDisoriented;
      }
      break;
    }

    default:
      /* stop robot */
      StopMotors();
      StopGate();
      break;
  }


}


void ExecutePowerSM(){
  // Wave at audiance when beginning mission.
  if (!WaveTimer.check()){
    if (no_of_waves%2 == 1){WaveUpAtAudience();}
    else {WaveDownAtAudience();}
  }
  else if (no_of_waves <= 5){
    WaveTimer.reset();
    no_of_waves++;
  }
  else {
    StopWave();
  }

  // Stop 
  if (MisssionTimer.check() and State!=MissionEnd){
    State = MissionEnd;
    WaveTimer.reset();
  }
}


void ExecuteRGBLightSM(){
  switch (State) {
    case AtStudioDisoriented:
    case AtStudioOrienting:
    case AtStudioOriented:
    case DumpingBalls:
    case MissionEnd:
    case ChooseBasket:
      setRGBcolor(RGB_BLANK);
      break;

    case FollowingRedTapeToBasket:
    case FollowingRedTapeToStudio:
      setRGBcolor(RGB_RED);
      break;

    case HeadingToGoodBasket:
      setRGBcolor(RGB_BLUE);
      break;

    case HeadingToBadBasket:
      setRGBcolor(RGB_PURPLE);
      break;

    case HeadingBackFromBadBasket:
      setRGBcolor(RGB_YELLOW);
      break;

    case HeadingBackFromGoodBasket:
      setRGBcolor(RGB_CYAN);
      break;

    case IgnoreRedTapeToBasket:
      setRGBcolor(RGB_YELLOW);
      break;

    case IgnoreRedTapeToStudio:
      setRGBcolor(RGB_WHITE);
      break;      

    default:
      break;
  }
}


void ExecuteSafetySM(){
  // TODO.
  // This SM should enforce state transitions of rotating in the case that the red tape
  // line is not detected while commencing towards either baskets from the studio.
}


// void LEDTimerHandler(){
//   ExecuteLEDandPotSM();

//   if (LED_state == HIGH){
//     LED_state = LOW;
//     analogWrite(LEDPin_out, map(pot_reading, 0, 1023, 0, 255));
//   }
//   else {
//     LED_state = HIGH;
//     analogWrite(LEDPin_out, 0);
//   }
// }


void ExecuteLEDandPotSM(){
  // pot_reading = map(analogRead(potPin_in), 0, 1023, 0, 255);
  pot_reading = analogRead(potPin_in);  // [in range 0, 1023]
  if (inRange(pot_reading, BAD_POT_CUTOFF, 1023)){
    // Serial.println("C1");
    Basket = BAD;
    Action = MOVE;
    // LED_freq = 10; // [Hz]
    LED_state = HIGH;
  }
  else if (inRange(pot_reading, 512, BAD_POT_CUTOFF)){
    // Serial.println("C2");
    Basket = BAD;
    Action = WAIT;
    // LED_freq = 50; // [Hz]
    LED_state = LOW;
  }
  else if (inRange(pot_reading, GOOD_POT_CUTOFF, 512)){
    // Serial.println("C3");
    Basket = GOOD;
    Action = WAIT;
    // LED_freq = 50; // [Hz]
    LED_state = LOW;
  }
  else {
    // Serial.println("C4");
    Basket = GOOD;
    Action = MOVE;
    // LED_freq = 10; // [Hz]
    LED_state = HIGH;
  }
  digitalWrite(LEDPin_out, LED_state);
}


void setRGBcolor(uint8_t color[]){
  digitalWrite(RGB_RED_PIN, color[0]);
  digitalWrite(RGB_GREEN_PIN, color[1]);
  digitalWrite(RGB_BLUE_PIN, color[2]);
}

