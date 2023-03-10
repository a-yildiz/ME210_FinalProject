#include <StateMachineFunctions.h>

// #define USE_TIMER_2     true
// #include <TimerInterrupt.h>



/* Initializations */
states State = AtStudioNotOriented; // AtStudioOrientDone; //FollowingRedTapeToBasket; //AtStudioNotOriented;
baskets Basket = GOOD;
actions Action = MOVE;

char Studio_ID = 'A';
bool LED_state = HIGH;
int LED_freq = 10;   // [Hz]
int no_of_waves = 0;

int pot_reading = 512;  // [out of 1023]
int beacon_reading = 1023;

int servo_angle = 0;

bool verbose_states = true;
int blind_follow_millis = 0;

Metro DebugTimer(750);
bool debug_param = false;

Metro PrintVarTimer(250);
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
  if (verbose_states){
    Serial.begin(9600);
    while(!Serial);
    Serial.println("Hello, world!");
  }

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
  
  StopMotors();
  RaiseGate();

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

  // PrintVar("State", State, PrintVarTimer);
  // PrintVar("Action", Action, PrintVarTimer);

  
  // if (DebugTimer.check()) {debug_param = true;}
  // if (debug_param) {StopMotors();}
  // else {MoveBackward(140, -30);}
  

  // TurnLeft(100, 10);

  // /* State Machines */
  ExecuteLEDandPotSM();
  ExecutePrimarySM();
    
  // ExecuteRGBLightSM();
  // ExecutePowerSM();
  // ExecuteSafetySM();

  // PrintVar("beacon", analogRead(beaconPin_in), PrintVarTimer);
  // if (beaconStrongEnough(analogRead(beaconPin_in))){
  //   Serial.println("Stop");
    // StopMotors();
  // }
  // else{
  // }
    


  // int beacon_val =  analogRead(beaconPin_in);
  // PrintVar("beacon val", beacon_val, PrintVarTimer);

  // PrintVar("pot reading", pot_reading, PrintVarTimer);
}


void ExecutePrimarySM(){
  switch(State) {

    case LINEFollowingToGoodBasket:{
      if (verbose_states) {PrintVar("State is PureRedLineFollowing", State, PrintVarTimer);}
      followLineFWD(120, 30, 0);
      // if ((detectLine(lineLeftPin_in)==BLACK) || (detectLine(lineRightPin_in)==BLACK)){
      //   PrintLineColors(lineLeftPin_in, lineRightPin_in);  
      //   State = StopIndefinitely;
      // }
      break;
    }

    case DumpingBalls:{
      if (verbose_states) {PrintVar("State is DumpingBalls", State, PrintVarTimer);}
      StopMotors();
      RaiseGate();

      if (StateTimer.check()){
        LowerGate();
        StateTimer.reset();

        if (Basket==BAD){
          State = HeadingBackFromBadBasket;
        }
        else {
          State = ROTATEInFrontOfBasket;
        }
      }
      break;
    }


    case ROTATEInFrontOfBasket:{
      if (verbose_states) {PrintVar("State is ROTATEInFrontOfBasket", State, PrintVarTimer);}
      MoveBackward(250, -110);
      RaiseGate();

      if (detectLine(lineLeftPin_in)==RED){
        State = LINEFollowingToStudioFromGoodBasket;
      }
      break;
      break;
    }


    case LINEFollowingToStudioFromGoodBasket:{
      if (verbose_states) {PrintVar("State is PureRedLineFollowing", State, PrintVarTimer);}
      followLineFWD(120, 0, -10);
      if ((detectLine(lineLeftPin_in)==BLACK) || (detectLine(lineRightPin_in)==BLACK)){
        PrintLineColors(lineLeftPin_in, lineRightPin_in);  
      }
      break;
    }





    case PureRedLineFollowing:{
      if (verbose_states) {PrintVar("State is PureRedLineFollowing", State, PrintVarTimer);}
      followRedLine(120, 0, -10, "fwd");
      if ((detectLine(lineLeftPin_in)==BLACK) || (detectLine(lineRightPin_in)==BLACK)){
        PrintLineColors(lineLeftPin_in, lineRightPin_in);
        // State = StopIndefinitely;
      }
      break;
    }

    case DebugLineSensors:{
      // if (verbose_states) {PrintVar("State is DebugLineSensors", State, PrintVarTimer);}
      PrintLineColors(lineLeftPin_in, lineRightPin_in, PrintVarTimer);
      StopMotors();
      break;
    }

    case AtStudioNotOriented:{
      if (verbose_states) {PrintVar("State is AtStudioNotOriented", State, PrintVarTimer);}
      if (PotentiometerInWait(Action)) {
        MotorPulse(StartRotatingCoM, 255, -30);
        StartRotatingCoM(110, -25);
        State = AtStudioOrienting;
      }
      break;
    }

    case AtStudioOrienting:{
      if (verbose_states) {PrintVar("State is AtStudioOrienting", State, PrintVarTimer);}
      int new_beacon_reading = analogRead(beaconPin_in);
      // Serial.println("Beacon reading:" + String(new_beacon_reading));
      if (beaconStrongEnough(new_beacon_reading)) {
      Serial.println("Stop. Beacon detected.");
        StopMotors();
        State = AtStudioOrientDone;
        StateTimer.reset();
      }
      beacon_reading = new_beacon_reading;
      break;
    }

    case AtStudioOrientDone:{
      if (verbose_states) {PrintVar("State is AtStudioOrientDone", State, PrintVarTimer);}
      int new_pot_reading = analogRead(potPin_in);

      // Reset timer if the pot reading has changed.
      if (!approx_equal(pot_reading, new_pot_reading)) {StateTimer.reset();}

      // If 1 sec has passed since last pot reading change, then proceed to the chosen basket.
      if (StateTimer.check()){  // && Action==MOVE){
        // State = ChooseBasket;
        State = XX_Stop1;
        StateTimer.interval(1000);
        StateTimer.reset();
      }
      pot_reading = new_pot_reading;
      // PrintVar("pot reading", pot_reading, PrintVarTimer);
      break;
    }



    case XX_Stop1:{
      if (verbose_states) {PrintVar("State is XX_Stop1", State, PrintVarTimer);}
      StopMotors();
      if (StateTimer.check()){
        State = XX_LineTrack1;
        StateTimer.interval(200);
        StateTimer.reset();
      }
      break;
    }


    case XX_LineTrack1:{
      MoveForward(130, -33);
      if (detectLine(lineLeftPin_in)==RED || detectLine(lineRightPin_in)==RED){
        State = LINEFollowingToGoodBasket;
      }
      break;
    }


    case XX_LineTrack2:{

      break;
    }

    case ChooseBasket:{
      if (verbose_states) {PrintVar("State is ChooseBasket", State, PrintVarTimer);}
      if (Basket==GOOD){
        // rotateAlphaGood();
        if (StateTimer.check()) {
          State = BlindlyForwardGoodBasket;
          MotorPulse(MoveForward, 180, -30);
          StateTimer.reset();          // blind_follow_millis = millis();
          }
      }
      else {
        // rotateAlphaBad();
        if (StateTimer.check()) {
          State = BlindlyForwardBadBasket;
          MotorPulse(MoveForward, 180, -30);
          StateTimer.reset();          // blind_follow_millis = millis();
          } 
      }
      break;
    }

    case BlindlyForwardBadBasket:{
      if (verbose_states) {PrintVar("State is BlindlyForwardBadBasket", State, PrintVarTimer);}
      if (StateTimer.check()){
        StopMotors();
        State = HeadingToBadBasket;
      }
      else{
        MoveForward(140, -33);  // - (right) vs + (left)
      }
      break;
    }


    case HeadingToBadBasket:{
      if (verbose_states) {PrintVar("State is HeadingToBadBasket", State, PrintVarTimer);}
      MoveForward(130, -33);
      
      if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
        PrintLineColors(lineLeftPin_in, lineRightPin_in);
        Serial.println("Red tape entered!");
        // State = DebugLineSensors;
        // State = RotateLeftToFindRedTape;
        State = StopBrieflyBeforeTurn;
        StateTimer.reset();
        StopMotors();
      }
      break;
    }

    case StopBrieflyBeforeTurn:{
      if (verbose_states) {PrintVar("State is StopBrieflyBeforeTurn", State, PrintVarTimer);}
      StopMotors();
      if (StateTimer.check()){
        Serial.println("Rotating left soon!");
        State = RotateLeftToFindRedTape;
        // StateTimer.interval(450);
        // StateTimer.reset();
      }
      break;
    }

    case RotateLeftToFindRedTape:{
      if (verbose_states) {PrintVar("State is RotateLeftToFindRedTape", State, PrintVarTimer);}
      RotateLeft(150, -30);
      
      if (detectLine(lineRightPin_in)==RED){
        Serial.println("Rotated left");
        // State = StopIndefinitely;
        State = FollowingRedTapeToBasket;
      }
      break;
    }

    case FollowingRedTapeToBasket:{
      if (verbose_states) {PrintVar("State is FollowingRedTapeToBasket", State, PrintVarTimer);}
      followRedLine(120, 0, -10, "fwd");
      if ((detectLine(lineLeftPin_in)==BLACK) || (detectLine(lineRightPin_in)==BLACK)){
        PrintLineColors(lineLeftPin_in, lineRightPin_in);
        // State = StopIndefinitely;
        StateTimer.interval(2000);
        StateTimer.reset();
        State = DumpingBalls;
      }
      break;
    }





  case HeadingBackFromBadBasket:{
      if (verbose_states) {PrintVar("State is HeadingBackFromBadBasket", State, PrintVarTimer);}
      MoveBackward(250, -110);
      RaiseGate();

      if (StateTimer.check()){
        State = BlindlyToRedTape;
        StateTimer.interval(1000);
        StateTimer.reset();
      }
      break;
  }

  case BlindlyToRedTape:{
      if (verbose_states) {PrintVar("State is BlindlyToRedTape", State, PrintVarTimer);}
      if (StateTimer.check()){
        StopMotors();
        State = HeadToVerticalRedTape;
      }
      else{
        MoveForward(140, -33);  // - (right) vs + (left)
      }
      break;
  }


  case HeadToVerticalRedTape:{
    if (verbose_states) {PrintVar("State is HeadingToBadBasket", State, PrintVarTimer);}
    MoveForward(130, -21);
    
    if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
      PrintLineColors(lineLeftPin_in, lineRightPin_in);
      Serial.println("Red tape entered!");
      State = RotateLeftToFindRedTape2;
      StateTimer.interval(300);
      StateTimer.reset();
      StopMotors();
    }
    break;
  }


  case RotateLeftToFindRedTape2:{
    if (verbose_states) {PrintVar("State is RotateLeftToFindRedTape", State, PrintVarTimer);}
    RotateLeft(140, -30);
    
    if (StateTimer.check() && (lineRightPin_in)==RED){
      Serial.println("Rotated left");
      State = StopIndefinitely;
    }
    break;
  }

  case Anil00_Init:{
    if (verbose_states) {PrintVar("State is Anil00_Init", State, PrintVarTimer);}
    State = Anil01_Fwd;
    StateTimer.interval(1300);
    StateTimer.reset();
    break;
  }


  case Anil01_Fwd:{
    if (verbose_states) {PrintVar("State is Anil01_Fwd", State, PrintVarTimer);}
    MoveForward(150, -30);
    if (StateTimer.check()){
      State = Anil02_Rotate;
      StateTimer.interval(440);
      StateTimer.reset();
    }
    break;
  }

  case Anil02_Rotate:{
    if (verbose_states) {PrintVar("State is Anil02_Rotate", State, PrintVarTimer);}
    RotateLeft(80, -30);
    if (StateTimer.check()){
      State = Anil03_StopBeforeBad;

    }
    break;
  }

  case Anil03_StopBeforeBad:{
    StopMotors();
    if (StateTimer.check()){
      State = Anil04_FwdToBad;
      StateTimer.interval(1450);
      StateTimer.reset();
    }
    break;
  }

  case Anil04_FwdToBad:{
    MoveForward(140, -30);
    if (StateTimer.check() || detectLine(lineLeftPin_in)==BLACK || detectLine(lineRightPin_in)==BLACK){
      State = RollBalls;
      StateTimer.interval(1000);
      StateTimer.reset();
    }
    break;
  }

  case RollBalls:{
    LowerGate();
    StopMotors();
    if (StateTimer.check()){
      State = Anil05_BackFromBad;
      StateTimer.interval(1000);
      StateTimer.reset();
    }
    break;
  }

  case Anil05_BackFromBad:{
    MoveBackward(140, -30);
    if (StateTimer.check()){
      State = Anil55_StopBeforeBad2;
      StateTimer.interval(1000);
      StateTimer.reset();
    }
    break;
  }

  case Anil55_StopBeforeBad2:{
    StopMotors();
    if (StateTimer.check()){
      State = StopIndefinitely;
    }
    break;
  }

  // case Anil06_RotateFromBadToGood:{
  //   RotateRight(80, -30);
  //   if (StateTimer.check()){
  //     State = StopIndefinitely;
  //   }
  //   break;
  // }





    case HeadingToGoodBasket:{
      if (verbose_states) {PrintVar("State is HeadingToGoodBasket", State, PrintVarTimer);}
      MoveForward(0);
      if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
        State = IgnoreRedTapeToBasket;
        StateTimer.reset();
      }
      break;
    }


    case IgnoreRedTapeToBasket:{
      if (verbose_states) {PrintVar("State is IgnoreRedTapeToBasket", State, PrintVarTimer);}
      if (StateTimer.check()){
        if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
          State = FollowingRedTapeToBasket;
        }
      }
      break;
    }

    case IgnoreRedTapeToStudio:{
      if (verbose_states) {PrintVar("State is IgnoreRedTapeToStudio", State, PrintVarTimer);}
      if (StateTimer.check()){
        if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
          State = FollowingRedTapeToStudio;
        }
      }
      break;
    }



    case HeadingBackFromGoodBasket:{
      if (verbose_states) {PrintVar("State is HeadingBackFromGoodBasket", State, PrintVarTimer);}
      if (StateTimer.check()){
        MoveForward(0);
        if ((detectLine(lineLeftPin_in)==RED) || (detectLine(lineRightPin_in)==RED)){
          State = IgnoreRedTapeToStudio;
          StateTimer.reset();
        }
      }
      break;
    }

    case FollowingRedTapeToStudio:{
      if (verbose_states) {PrintVar("State is FollowingRedTapeToStudio", State, PrintVarTimer);}
      int beacon_val = analogRead(beaconPin_in);
      if (beaconStrongEnough(beacon_val)){
        StopMotors();
        State = AtStudioNotOriented;
      }
      break;
    }

    case StopIndefinitely:{
      if (verbose_states) {PrintVar("State is StopIndefinitely", State, PrintVarTimer);}
      StopMotors();
      RaiseGate();
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
    case AtStudioNotOriented:
    case AtStudioOrienting:
    case AtStudioOrientDone:
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
  // Serial.println(pot_reading);
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

