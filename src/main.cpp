#include <StateMachineFunctions.h>

// #define USE_TIMER_2     true
// #include <TimerInterrupt.h>

/* Initializations */
states State = AtStudioNotOriented; // AtStudioOrientDone; //FollowingRedTapeToBasket; //AtStudioNotOriented;
baskets Basket = GOOD;
actions Action = MOVE;

char Studio_ID = 'A';
bool LED_state = HIGH;
int LED_freq = 10; // [Hz]
int no_of_waves = 0;

int pot_reading = 512; // [out of 1023]
int beacon_reading = 1023;

int countTurns = 0;

int rotateStart = 0;
int rotateEnd = 0;
int last_beacon_reading = 0;

bool leftTripped = false;
bool rightTripped = false;
bool thirdTripped = false;
bool ready = false;

bool rotatingLeft = true;
bool centering = false;
int searchInterval = 200;
int maxSearchInterval = 600;

bool lastTurningRight = false;

bool hitJunction = false;

bool dumped = false;
int dumpcount = 0;

int servo_angle = 0;

bool verbose_states = true;
int blind_follow_millis = 0;

Metro DebugTimer(750);
bool debug_param = false;

Metro PrintVarTimer(250);
Metro StateTimer(1000);
Metro WaveTimer(2000);
Metro MisssionTimer(130000); // Mission time: 2m 10s = 130s = 130000ms

/* Function Definitions */
void ExecuteLEDandPotSM();
void ExecutePrimarySM();
void ExecutePowerSM();
void ExecuteRGBLightSM();
void ExecuteSafetySM();

// void LEDTimerHandler();
// void DummyHandler();
void setRGBcolor(uint8_t color[]);

void setup()
{
  /* DEBUG */
  if (verbose_states)
  {
    Serial.begin(9600);
    while (!Serial)
      ;
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

void loop()
{
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

void ExecutePrimarySM()
{
  switch (State)
  {

  case LINEFollowingToGoodBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is LINEFollowingToGoodBasket", State, PrintVarTimer);
    }
    followLineFWD(Basket, 120, 30, 0);
    if ((detectLine(lineLeftPin_in, 500, 700) == BLACK) || (detectLine(lineRightPin_in, 650, 763) == BLACK))
    {
      // PrintLineColors(lineLeftPin_in, lineRightPin_in);
      State = DumpingBalls; // StopIndefinitely;
      StateTimer.interval(2000);
      StateTimer.reset();
    }
    break;
  }

  case DumpingBalls:
  {
    if (verbose_states)
    {
      PrintVar("State is DumpingBalls", State, PrintVarTimer);
    }
    StopMotors();
    LowerGate();

    if (StateTimer.check())
    {
      RaiseGate();
      State = RotateToRedLine;
      StateTimer.interval(500);
      StateTimer.reset();
    }
    break;
  }

  case RotateToRedLine:
  {
    if (verbose_states)
    {
      PrintVar("State is RotateToRedLine", State, PrintVarTimer);
    }
    RotateLeft(100, 10);
    if (StateTimer.check() && detectLine(lineLeftPin_in, 500, 700) == RED)
    {
      State = LINEFollowingToStudioFromGoodBasket;
    }
    break;
  }

  case LINEFollowingToStudioFromGoodBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is LINEFollowingToStudioFromGoodBasket", State, PrintVarTimer);
    }
    followLineFWD(Basket, 120, 30, 0);
    if ((detectLine(lineLeftPin_in, 500, 700) == BLACK) || (detectLine(lineRightPin_in, 650, 763) == BLACK))
    {
      // PrintLineColors(lineLeftPin_in, lineRightPin_in);
      State = StopIndefinitely;
    }
    break;
  }

  case DebugLineSensors:
  {
    // if (verbose_states) {PrintVar("State is DebugLineSensors", State, PrintVarTimer);}
    PrintLineColors(lineLeftPin_in, lineRightPin_in, lineThirdPin_in, PrintVarTimer);
    StopMotors();
    break;
  }

  case AtStudioNotOriented:
  {
    if (verbose_states)
    {
      PrintVar("State is AtStudioNotOriented", State, PrintVarTimer);
    }
    if (PotentiometerInWait(Action))
    {
      rotateStart = millis();
      MotorPulse(StartRotatingCoM, 255, -30);
      StartRotatingCoM(180, -25);
      State = AtStudioOrienting;
    }
    break;
  }

  case AtStudioOrienting:
  {
    if (verbose_states)
    {
      PrintVar("State is AtStudioOrienting", State, PrintVarTimer);
    }
    int new_beacon_reading = analogRead(beaconPin_in);
    // Serial.println("Beacon reading:" + String(new_beacon_reading));
    if (beaconStrongEnough(new_beacon_reading))
    {

      rotateEnd = millis();
      last_beacon_reading = new_beacon_reading;
      Serial.print("Stop. Beacon detected after ");
      Serial.print(rotateEnd - rotateStart);
      Serial.println("ms");
      StopMotors();
      MotorPulse(StartRotatingCoMReverse, 255, -30, 40 + .2 * min((rotateEnd - rotateStart), 630));
      StopMotors();
      State = AtStudioOrientAdjust;
      StateTimer.reset();
    }
    beacon_reading = new_beacon_reading;
    break;
  }

  case AtStudioOrientAdjust:
  {
    if (verbose_states)
    {
      PrintVar("State is AtStudioOrientAdjust", State, PrintVarTimer);
    }
    // Rotate counterclockwise until the beacon decreases strength or exceeds threshhold
    MotorPulse(StartRotatingCoMReverse, 0, 0, 800);
    countTurns += 1;
    if (countTurns > 7)
    {
      rotateStart = millis();
      MotorPulse(StartRotatingCoM, 255, -30);
      StartRotatingCoM(180, -25);
      State = AtStudioOrienting;
      countTurns = 0;
      break;
    }

    int new_beacon_reading = analogRead(beaconPin_in);
    if (beaconStrongEnough(new_beacon_reading - 30))
    {
      Serial.print("Done!");
      Serial.println("Stop. Beacon locked.");
      StopMotors();
      State = AtStudioOrientDone;
      StateTimer.reset();
    }

    MotorPulse(StartRotatingCoMReverse, 180, -30, 70);

    // delay(1000);
    // StartRotatingCoMReverse(100, -25);
    // if (beaconStrongEnough(new_beacon_reading))
    // {
    //   Serial.println("Stop. Beacon detected.");
    //   StopMotors();
    //   State = AtStudioOrientDone;
    //   StateTimer.reset();
    // }
    // beacon_reading = new_beacon_reading;
    break;
  }

  case AtStudioOrientDone:
  {
    if (verbose_states)
    {
      PrintVar("State is AtStudioOrientDone", State, PrintVarTimer);
    }
    int new_pot_reading = analogRead(potPin_in);

    // Reset timer if the pot reading has changed.
    if (!approx_equal(pot_reading, new_pot_reading))
    {
      StateTimer.reset();
    }

    // If 1 sec has passed since last pot reading change, then proceed to the chosen basket.
    if (StateTimer.check() || true)
    { // && Action==MOVE){
      // State = ChooseBasket;
      State = AtStudioLiningUp;
      MoveForward(130, -33);
      StateTimer.interval(1000);
      StateTimer.reset();
    }
    pot_reading = new_pot_reading;
    // PrintVar("pot reading", pot_reading, PrintVarTimer);
    break;
  }

  case AtStudioLiningUp:
  {

    Serial.print(analogRead(lineRightPin_in));
    Serial.print(" - ");
    Serial.print(detectLine(lineRightPin_in, 650, 763));
    Serial.print(" - ");
    Serial.print(analogRead(lineLeftPin_in));
    Serial.print(" - ");
    Serial.print(detectLine(lineLeftPin_in, 500, 700));
    Serial.print(" - ");
    Serial.print(analogRead(lineThirdPin_in));
    Serial.print(" - ");
    Serial.println(detectLine(lineThirdPin_in, 650, 700));
    if (verbose_states)
    {
      PrintVar("State is AtStudioLiningUp", State, PrintVarTimer);
    }

    if (!leftTripped && (detectLine(lineLeftPin_in, 500, 700) == BLACK))
    {

      // StopMotors();
      Serial.println("Left tripped");
      leftTripped = true;
    }

    if (!rightTripped && (detectLine(lineRightPin_in, 650, 763) == BLACK))
    {
      // StopMotors();
      Serial.println("Right tripped");
      rightTripped = true;
    }

    if (!thirdTripped && (detectLine(lineThirdPin_in, 620, 700) == BLACK))
    {
      // StopMotors();
      Serial.println("Third tripped");
      thirdTripped = true;
    }

    if (!ready && rightTripped && leftTripped && thirdTripped && detectLine(lineRightPin_in, 650, 763) != BLACK && detectLine(lineLeftPin_in, 500, 700) != BLACK && detectLine(lineThirdPin_in, 580, 700) != BLACK)
    {

      StateTimer.interval(400);
      StateTimer.reset();
      Serial.println("DONE");
      StopMotors();
      ready = true;
    }
    if (StateTimer.check() && ready)
    {
      State = FindingLine;
      StateTimer.interval(searchInterval);
      StateTimer.reset();
    }

    break;
  }

  case FindingLine:
  {
    if (verbose_states)
    {
      PrintVar("State is FindingLine", State, PrintVarTimer);
    }
    if (rotatingLeft)
    {
      StartRotatingCoM(140, -30);
    }
    else
    {
      StartRotatingCoMReverse(140, -30);
    }
    if (detectLine(lineLeftPin_in, 500, 700) == RED || detectLine(lineRightPin_in, 650, 763) == RED || detectLine(lineThirdPin_in, 620, 700) == RED)
    {
      Serial.println("Found line!");
      StopMotors();
      State = CenteringLine;
    }
    if (StateTimer.check())
    {
      if (centering)
      {
        searchInterval = min(searchInterval + 50, maxSearchInterval);
      }
      else
      {
        rotatingLeft = !rotatingLeft;
      }
      centering = !centering;
      StateTimer.reset();
    }
    // rotate left for x time or until anything hits red
    // if time expires, go back to center and switch direction and try again
    // if time expires a third time, center, go forward, increase x with a specified max
    // stop once something finds line

    // MoveForward(160, -33);
    // if (StateTimer.check())
    // {
    //   State = LINEFollowingToGoodBasket;
    // }
    break;
  }

  case CenteringLine:
  {
    if (detectLine(lineThirdPin_in, 620, 700) == RED)
    {
      StartRotatingCoMReverse(140, -30);
    }
    if (detectLine(lineLeftPin_in, 500, 700) == RED || detectLine(lineRightPin_in, 650, 763) == RED)
    {
      StopMotors();
      State = FollowLineToGood;
    }
    break;
  }

  case FollowLineToGood:
  {

    dumped = false;
    if (detectLine(lineRightPin_in, 650, 763) == RED || (detectLine(lineLeftPin_in, 500, 700) != RED && lastTurningRight))
    {
      // if (!fixingAngle && StateTimer.check())
      // {
      // MotorPulse(MoveBackward, 255, -30, 60);
      // MotorPulse(StopMotors, 0, 0, 100);
      // }
      // Serial.println("Going right because it's on the left");
      // StopMotors();
      // TurnRight(120, -30);
      MotorPulse(TurnRight, 175, -30, 19);
      MotorPulse(StopMotors, 0, 0, 18);
      // MotorPulse(StartRotatingCoM, 140, -30, 15);
      // TurnRight(135, -30);
      lastTurningRight = true;
    }
    else
    // else if (detectLine(lineLeftPin_in, 500, 700) == RED)
    {
      // if (fixingAngle && StateTimer.check())
      // {
      // MotorPulse(MoveBackward, 255, -30, 60);
      // MotorPulse(StopMotors, 0, 0, 100);
      // }
      // Serial.println("Going left because it's on the left");
      // StopMotors();
      // TurnLeft(135, -30);
      // TurnLeft(120, -30);
      MotorPulse(TurnLeft, 175, -30, 19);
      MotorPulse(StopMotors, 0, 0, 18);
      // MotorPulse(StartRotatingCoMReverse, 140, -30, 15);
      lastTurningRight = false;
    }
    // else
    // {
    //   if (fixingAngle)
    //   {
    //     fixingAngle = false;
    //     MotorPulse(StopMotors, 0, 0, 100);
    //     StateTimer.interval(400);
    //     StateTimer.reset();
    //   }
    //   // Serial.println("Going straight bc no red line seen");
    //   MoveForward(135, -30);
    // }

    if (!hitJunction && detectLine(lineThirdPin_in, 620, 700) == RED)
    {
      MotorPulse(RotateLeft, 175, -30, 300);
      StopMotors();
      lastTurningRight = true;
      hitJunction = true;
    }

    if (detectLine(lineRightPin_in, 650, 763) == BLACK || (detectLine(lineLeftPin_in, 500, 700) == BLACK))
    {
      if (dumpcount == 2)
      {
        MotorPulse(MoveForward, 140, -30, 100);
        StopMotors();
        delay(3000);
        dumpcount = 0;
        State = AtStudioNotOriented;
        break;
      }
      StopMotors();
      hitJunction = false;
      State = DumpAndTurn;
    }
    break;
  }

  case DumpAndTurn:
  {
    if (!dumped)
    {
      Serial.println("Dumping and turning");
      FlickerGate();
      delay(2000);
      MotorPulse(MoveBackward, 180, -30, 200);
      MotorPulse(StartRotatingCoMReverse, 180, -30, 180);
      dumpcount += 1;
      dumped = true;
    }
    StartRotatingCoMReverse(135, -30);
    if (detectLine(lineLeftPin_in, 500, 700) == RED || detectLine(lineRightPin_in, 650, 763) == RED)
    {
      StopMotors();
      State = FollowLineToGood;
    }
    // {
    //   Serial.println("Detected line");
    //   StopMotors();
    //   MotorPulse(StartRotatingCoM, 200, -30, 20);
    //   StopMotors();
    //   // MoveForward(160, -30);
    //   State = FollowLineToGood;
    // }
    break;
  }

  case LineFollowToBad:
    /*
    detectLine(lineThirdPin_in, 620, 700)
    detectLine(lineRightPin_in, 650, 763)
    detectLine(lineLeftPin_in, 500, 700)

    */

    {
      if (detectLine(lineLeftPin_in, 500, 700) == RED || (detectLine(lineThirdPin_in, 620, 700) != RED && lastTurningRight))
      {
        // if (!fixingAngle && StateTimer.check())
        // {
        // MotorPulse(MoveBackward, 255, -30, 60);
        // MotorPulse(StopMotors, 0, 0, 100);
        // }
        // Serial.println("Going right because it's on the left");
        // StopMotors();
        // TurnRight(120, -30);
        MotorPulse(TurnRight, 175, -30, 19);
        MotorPulse(StopMotors, 0, 0, 18);
        // MotorPulse(StartRotatingCoM, 140, -30, 15);
        // TurnRight(135, -30);
        lastTurningRight = true;
      }
      else
      // else if (detectLine(lineLeftPin_in, 500, 700) == RED)
      {
        // if (fixingAngle && StateTimer.check())
        // {
        // MotorPulse(MoveBackward, 255, -30, 60);
        // MotorPulse(StopMotors, 0, 0, 100);
        // }
        // Serial.println("Going left because it's on the left");
        // StopMotors();
        // TurnLeft(135, -30);
        // TurnLeft(120, -30);
        MotorPulse(TurnLeft, 175, -30, 19);
        MotorPulse(StopMotors, 0, 0, 18);
        // MotorPulse(StartRotatingCoMReverse, 140, -30, 15);
        lastTurningRight = false;
      }
      // else
      // {
      //   if (fixingAngle)
      //   {
      //     fixingAngle = false;
      //     MotorPulse(StopMotors, 0, 0, 100);
      //     StateTimer.interval(400);
      //     StateTimer.reset();
      //   }
      //   // Serial.println("Going straight bc no red line seen");
      //   MoveForward(135, -30);
      // }
      if (detectLine(lineRightPin_in, 650, 763) == RED)
      {
        StopMotors();
        State = TurnAtJunction;
      }
      break;
    }

  case XX_Stop1:
  {
    if (verbose_states)
    {
      PrintVar("State is XX_Stop1", State, PrintVarTimer);
    }
    StopMotors();
    if (StateTimer.check())
    {
      State = XX_LineTrack1;
      StateTimer.interval(500);
      StateTimer.reset();
      MotorPulse(MoveForward, 255, -30);
    }
    break;
  }

  case XX_LineTrack1:
  {
    if (verbose_states)
    {
      PrintVar("State is XX_LineTrack1", State, PrintVarTimer);
    }
    MoveForward(160, -33);
    if (StateTimer.check())
    {
      State = LINEFollowingToGoodBasket;
    }
    break;
  }

  case ChooseBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is ChooseBasket", State, PrintVarTimer);
    }
    if (Basket == GOOD)
    {
      // rotateAlphaGood();
      if (StateTimer.check())
      {
        State = BlindlyForwardGoodBasket;
        MotorPulse(MoveForward, 180, -30);
        StateTimer.reset(); // blind_follow_millis = millis();
      }
    }
    else
    {
      // rotateAlphaBad();
      if (StateTimer.check())
      {
        State = BlindlyForwardBadBasket;
        MotorPulse(MoveForward, 180, -30);
        StateTimer.reset(); // blind_follow_millis = millis();
      }
    }
    break;
  }

  case BlindlyForwardBadBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is BlindlyForwardBadBasket", State, PrintVarTimer);
    }
    if (StateTimer.check())
    {
      StopMotors();
      State = HeadingToBadBasket;
    }
    else
    {
      MoveForward(140, -33); // - (right) vs + (left)
    }
    break;
  }

  case HeadingToBadBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is HeadingToBadBasket", State, PrintVarTimer);
    }
    MoveForward(130, -33);

    if ((detectLine(lineLeftPin_in, 500, 700) == RED) || (detectLine(lineRightPin_in, 650, 763) == RED))
    {
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

  case StopBrieflyBeforeTurn:
  {
    if (verbose_states)
    {
      PrintVar("State is StopBrieflyBeforeTurn", State, PrintVarTimer);
    }
    StopMotors();
    if (StateTimer.check())
    {
      Serial.println("Rotating left soon!");
      State = RotateLeftToFindRedTape;
      // StateTimer.interval(450);
      // StateTimer.reset();
    }
    break;
  }

  case RotateLeftToFindRedTape:
  {
    if (verbose_states)
    {
      PrintVar("State is RotateLeftToFindRedTape", State, PrintVarTimer);
    }
    RotateLeft(150, -30);

    if (detectLine(lineRightPin_in, 650, 763) == RED)
    {
      Serial.println("Rotated left");
      // State = StopIndefinitely;
      State = FollowingRedTapeToBasket;
    }
    break;
  }

  case FollowingRedTapeToBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is FollowingRedTapeToBasket", State, PrintVarTimer);
    }
    followRedLine(120, 0, -10, "fwd");
    if ((detectLine(lineLeftPin_in, 500, 700) == BLACK) || (detectLine(lineRightPin_in, 650, 763) == BLACK))
    {
      PrintLineColors(lineLeftPin_in, lineRightPin_in);
      // State = StopIndefinitely;
      StateTimer.interval(2000);
      StateTimer.reset();
      State = DumpingBalls;
    }
    break;
  }

  case HeadingBackFromBadBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is HeadingBackFromBadBasket", State, PrintVarTimer);
    }
    MoveBackward(250, -110);
    RaiseGate();

    if (StateTimer.check())
    {
      State = BlindlyToRedTape;
      StateTimer.interval(1000);
      StateTimer.reset();
    }
    break;
  }

  case BlindlyToRedTape:
  {
    if (verbose_states)
    {
      PrintVar("State is BlindlyToRedTape", State, PrintVarTimer);
    }
    if (StateTimer.check())
    {
      StopMotors();
      State = HeadToVerticalRedTape;
    }
    else
    {
      MoveForward(140, -33); // - (right) vs + (left)
    }
    break;
  }

  case HeadToVerticalRedTape:
  {
    if (verbose_states)
    {
      PrintVar("State is HeadingToBadBasket", State, PrintVarTimer);
    }
    MoveForward(130, -21);

    if ((detectLine(lineLeftPin_in, 500, 700) == RED) || (detectLine(lineRightPin_in, 650, 763) == RED))
    {
      PrintLineColors(lineLeftPin_in, lineRightPin_in);
      Serial.println("Red tape entered!");
      State = RotateLeftToFindRedTape2;
      StateTimer.interval(300);
      StateTimer.reset();
      StopMotors();
    }
    break;
  }

  case RotateLeftToFindRedTape2:
  {
    if (verbose_states)
    {
      PrintVar("State is RotateLeftToFindRedTape", State, PrintVarTimer);
    }
    RotateLeft(140, -30);

    if (StateTimer.check() && (lineRightPin_in) == RED)
    {
      Serial.println("Rotated left");
      State = StopIndefinitely;
    }
    break;
  }

  case Anil00_Init:
  {
    if (verbose_states)
    {
      PrintVar("State is Anil00_Init", State, PrintVarTimer);
    }
    State = Anil01_Fwd;
    StateTimer.interval(1300);
    StateTimer.reset();
    break;
  }

  case Anil01_Fwd:
  {
    if (verbose_states)
    {
      PrintVar("State is Anil01_Fwd", State, PrintVarTimer);
    }
    MoveForward(150, -30);
    if (StateTimer.check())
    {
      State = Anil02_Rotate;
      StateTimer.interval(440);
      StateTimer.reset();
    }
    break;
  }

  case Anil02_Rotate:
  {
    if (verbose_states)
    {
      PrintVar("State is Anil02_Rotate", State, PrintVarTimer);
    }
    RotateLeft(80, -30);
    if (StateTimer.check())
    {
      State = Anil03_StopBeforeBad;
    }
    break;
  }

  case Anil03_StopBeforeBad:
  {
    StopMotors();
    if (StateTimer.check())
    {
      State = Anil04_FwdToBad;
      StateTimer.interval(1450);
      StateTimer.reset();
    }
    break;
  }

  case Anil04_FwdToBad:
  {
    MoveForward(140, -30);
    if (StateTimer.check() || detectLine(lineLeftPin_in, 500, 700) == BLACK || detectLine(lineRightPin_in, 650, 763) == BLACK)
    {
      State = RollBalls;
      StateTimer.interval(1000);
      StateTimer.reset();
    }
    break;
  }

  case RollBalls:
  {
    LowerGate();
    StopMotors();
    if (StateTimer.check())
    {
      State = Anil05_BackFromBad;
      StateTimer.interval(1000);
      StateTimer.reset();
    }
    break;
  }

  case Anil05_BackFromBad:
  {
    MoveBackward(140, -30);
    if (StateTimer.check())
    {
      State = Anil55_StopBeforeBad2;
      StateTimer.interval(1000);
      StateTimer.reset();
    }
    break;
  }

  case Anil55_StopBeforeBad2:
  {
    StopMotors();
    if (StateTimer.check())
    {
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

  case HeadingToGoodBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is HeadingToGoodBasket", State, PrintVarTimer);
    }
    MoveForward(0);
    if ((detectLine(lineLeftPin_in, 500, 700) == RED) || (detectLine(lineRightPin_in, 650, 763) == RED))
    {
      State = IgnoreRedTapeToBasket;
      StateTimer.reset();
    }
    break;
  }

  case IgnoreRedTapeToBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is IgnoreRedTapeToBasket", State, PrintVarTimer);
    }
    if (StateTimer.check())
    {
      if ((detectLine(lineLeftPin_in, 500, 700) == RED) || (detectLine(lineRightPin_in, 650, 763) == RED))
      {
        State = FollowingRedTapeToBasket;
      }
    }
    break;
  }

  case IgnoreRedTapeToStudio:
  {
    if (verbose_states)
    {
      PrintVar("State is IgnoreRedTapeToStudio", State, PrintVarTimer);
    }
    if (StateTimer.check())
    {
      if ((detectLine(lineLeftPin_in, 500, 700) == RED) || (detectLine(lineRightPin_in, 650, 763) == RED))
      {
        State = FollowingRedTapeToStudio;
      }
    }
    break;
  }

  case HeadingBackFromGoodBasket:
  {
    if (verbose_states)
    {
      PrintVar("State is HeadingBackFromGoodBasket", State, PrintVarTimer);
    }
    if (StateTimer.check())
    {
      MoveForward(0);
      if ((detectLine(lineLeftPin_in, 500, 700) == RED) || (detectLine(lineRightPin_in, 650, 763) == RED))
      {
        State = IgnoreRedTapeToStudio;
        StateTimer.reset();
      }
    }
    break;
  }

  case FollowingRedTapeToStudio:
  {
    if (verbose_states)
    {
      PrintVar("State is FollowingRedTapeToStudio", State, PrintVarTimer);
    }
    int beacon_val = analogRead(beaconPin_in);
    if (beaconStrongEnough(beacon_val))
    {
      StopMotors();
      State = AtStudioNotOriented;
    }
    break;
  }

  case StopIndefinitely:
  {
    if (verbose_states)
    {
      PrintVar("State is StopIndefinitely", State, PrintVarTimer);
    }
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

void ExecutePowerSM()
{
  // Wave at audiance when beginning mission.
  if (!WaveTimer.check())
  {
    if (no_of_waves % 2 == 1)
    {
      WaveUpAtAudience();
    }
    else
    {
      WaveDownAtAudience();
    }
  }
  else if (no_of_waves <= 5)
  {
    WaveTimer.reset();
    no_of_waves++;
  }
  else
  {
    StopWave();
  }

  // Stop
  if (MisssionTimer.check() and State != MissionEnd)
  {
    State = MissionEnd;
    WaveTimer.reset();
  }
}

void ExecuteRGBLightSM()
{
  switch (State)
  {
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

void ExecuteSafetySM()
{
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

void ExecuteLEDandPotSM()
{
  // pot_reading = map(analogRead(potPin_in), 0, 1023, 0, 255);
  pot_reading = analogRead(potPin_in); // [in range 0, 1023]
  // Serial.println(pot_reading);
  if (inRange(pot_reading, BAD_POT_CUTOFF, 1023))
  {
    // Serial.println("C1");
    Basket = BAD;
    Action = MOVE;
    // LED_freq = 10; // [Hz]
    LED_state = HIGH;
  }
  else if (inRange(pot_reading, 512, BAD_POT_CUTOFF))
  {
    // Serial.println("C2");
    Basket = BAD;
    Action = WAIT;
    // LED_freq = 50; // [Hz]
    LED_state = LOW;
  }
  else if (inRange(pot_reading, GOOD_POT_CUTOFF, 512))
  {
    // Serial.println("C3");
    Basket = GOOD;
    Action = WAIT;
    // LED_freq = 50; // [Hz]
    LED_state = LOW;
  }
  else
  {
    // Serial.println("C4");
    Basket = GOOD;
    Action = MOVE;
    // LED_freq = 10; // [Hz]
    LED_state = HIGH;
  }
  digitalWrite(LEDPin_out, LED_state);
}

void setRGBcolor(uint8_t color[])
{
  digitalWrite(RGB_RED_PIN, color[0]);
  digitalWrite(RGB_GREEN_PIN, color[1]);
  digitalWrite(RGB_BLUE_PIN, color[2]);
}
