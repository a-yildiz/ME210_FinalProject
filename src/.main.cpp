#include <StateMachineFunctions.h>

#define USE_TIMER_2 true
#include <TimerInterrupt.h>

#include <Metro.h>

/* brian's shit code*/
#define DIR_A_1 5
#define DIR_A_2 7
#define PWM_A 6
#define PWM_OFFSET_A 0
#define FORWARD_A true
#define DIR_B_1 2
#define DIR_B_2 4
#define PWM_B 3
#define PWM_OFFSET_B -30
#define POWER_STOP_OFF 4
#define FORWARD_B false
int speed_A = 0;
int speed_B = 0;
int default_speed = 150;

void RespToKey(void)
{
  uint8_t theKey;
  theKey = Serial.read();
  // Serial.print(theKey);
  // Serial.print(", ASCII=");
  // Serial.println(theKey, HEX);

  if (theKey == 0x77) // w
  {
    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = default_speed + PWM_OFFSET_A;
    speed_B = default_speed + PWM_OFFSET_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
    // Serial.println("FORWARD");
  }
  else if (theKey == 0x64) // d
  {

    digitalWrite(DIR_A_1, FORWARD_A);
    digitalWrite(DIR_A_2, !FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = default_speed + PWM_OFFSET_A;
    speed_B = default_speed + PWM_OFFSET_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
    // Serial.println("ROTATE RIGHT");
  }
  else if (theKey == 0x61) // a
  {

    digitalWrite(DIR_A_1, !FORWARD_A);
    digitalWrite(DIR_A_2, FORWARD_A);
    digitalWrite(DIR_B_1, FORWARD_B);
    digitalWrite(DIR_B_2, !FORWARD_B);
    speed_A = default_speed + PWM_OFFSET_A;
    speed_B = default_speed + PWM_OFFSET_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
    // Serial.println("ROTATE LEFT");
  }
  else if (theKey == 0x73) // s
  {
    digitalWrite(DIR_A_1, HIGH);
    digitalWrite(DIR_A_2, HIGH);
    digitalWrite(DIR_B_1, HIGH);
    digitalWrite(DIR_B_2, HIGH);
    speed_A = 255;
    speed_B = 255;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
    // Serial.println("STOP");
  }
  else if (theKey == 0x78) // x
  {
    digitalWrite(DIR_A_1, !FORWARD_A);
    digitalWrite(DIR_A_2, FORWARD_A);
    digitalWrite(DIR_B_1, !FORWARD_B);
    digitalWrite(DIR_B_2, FORWARD_B);
    speed_A = default_speed + PWM_OFFSET_A;
    speed_B = default_speed + PWM_OFFSET_B;
    analogWrite(PWM_A, speed_A);
    analogWrite(PWM_B, speed_B);
    // Serial.println("BACKWARD");
  }
  else if (theKey == 0x74) // t
  {
    default_speed = min(default_speed + 5, 255);
    Serial.print("Speed: ");
    Serial.println(default_speed);
  }
  else if (theKey == 0x67) // g
  {
    default_speed = max(default_speed - 5, 0);
    Serial.print("Speed: ");
    Serial.println(default_speed);
  }
  else if (theKey == 0x68) // h
  {
    LowerGate();
  }
  else if (theKey == 0x69) // i
  {
    RaiseGate();
  }
}

/* brian's shit code*/

/* Initializations */
states State = AtStudioNotOriented;
baskets Basket = GOOD;
actions Action = MOVE;

char Studio_ID = 'A';
bool LED_state = HIGH;
int LED_freq = 10; // [Hz]
int no_of_waves = 0;

int pot_reading = 512; // [out of 1023]
int beacon_reading = 1023;

int servo_angle = 0;

Metro StateTimer(1000);
Metro WaveTimer(2000);
Metro MisssionTimer(130000); // Mission time: 2m 10s = 130s = 130000ms

/* Function Definitions */
void ExecutePrimarySM();
void ExecutePowerSM();
void ExecuteRGBLightSM();
void ExecuteSafetySM();

void LEDTimerHandler();
void DummyHandler();
void ExecuteLEDandPotSM();
void setRGBcolor(uint8_t color[]);

void setup()
{
  /* DEBUG */
  Serial.begin(9600);
  while (!Serial)
    ;
  // Serial.println("Hello, world!");

  /* brian's shit code*/
  Serial.println("=========== [BOOT] ===========");

  pinMode(POWER_STOP_OFF, OUTPUT);
  digitalWrite(POWER_STOP_OFF, HIGH);
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
  /* brian's shit code*/

  /* Pins */
  // pinMode(potPin_in, INPUT);
  // pinMode(beaconPin_in, INPUT);
  // pinMode(lineLeftPin_in, INPUT);
  // pinMode(lineRightPin_in, INPUT);
  // pinMode(gateServoPin_out, OUTPUT);

  // pinMode(spstPin_in, INPUT);
  // pinMode(LEDPin_out, OUTPUT);

  // pinMode(RGB_RED_PIN, OUTPUT);
  // pinMode(RGB_GREEN_PIN, OUTPUT);
  // pinMode(RGB_BLUE_PIN, OUTPUT);

  // pinMode(bridge1Pin_pwm_out, OUTPUT);
  // pinMode(bridge2Pin_pwm_out, OUTPUT);

  // pinMode(bridge1Pin_dir_out, OUTPUT);
  // pinMode(bridge2Pin_dir_out, OUTPUT);

  // digitalWrite(gateServoPin_out, LOW);

  GateServo.attach(gateServoPin_pwm_out);

  // ITimer2.init();
  // ITimer2.attachInterrupt(LED_freq, LEDTimerHandler);

  // WaveTimer.reset();
  // MisssionTimer.reset();

  GateServo.write(0);
}

void loop()
{
  /* DEBUG */
  // PrintVar("State", State);
  // PrintVar("Action", Action);
  // delay(500);

  /* Detect Studio A or B from IR sensor. */
  // Studio_ID = detectStudioID(spstPin_in);
  // PrintVar("studio val", Studio_ID);

  /* Set LED brightness w.r.t. potentiometer reading. */
  // ITimer2.setFrequency(LED_freq, LEDTimerHandler);

  /* State Machines */
  // ExecutePrimarySM();
  // ExecuteRGBLightSM();
  // ExecutePowerSM();
  // ExecuteSafetySM();

  // int beacon_val = analogRead(beaconPin_in);
  if (Serial.available())
  {
    RespToKey();
    // char f = Serial.read();
    // PrintVar("right", detectLine(lineRightPin_in));
    // PrintVar("left", detectLine(lineLeftPin_in));
  }

  // RaiseGate();
  // LowerGate();

  // GateServo.write(180);
  // delay(500);
  // GateServo.write(60);
  // delay(500);
  // GateServo.write(90);
  // delay(500);
  // GateServo.write(-180);
  // delay(1);
  delayMicroseconds(1);
}

void ExecutePrimarySM()
{
  switch (State)
  {
  case AtStudioNotOriented:
  {
    if (PotentiometerInWait(Action))
    {
      StartRotatingCoM();
      State = AtStudioOrienting;
    }
    break;
  }

  case AtStudioOrienting:
  {
    int new_beacon_reading = analogRead(beaconPin_in);
    if (beaconStrongEnough(new_beacon_reading))
    {
      StopMotors();
      State = AtStudioOrientDone;
      StateTimer.reset();
    }
    beacon_reading = new_beacon_reading;
    break;
  }

  case AtStudioOrientDone:
  {
    int new_pot_reading = analogRead(potPin_in);

    // Reset timer if the pot reading has changed.
    if (!approx_equal(pot_reading, new_pot_reading))
    {
      StateTimer.reset();
    }

    // If 1 sec has passed since last pot reading change, then proceed to the chosen basket.
    if (StateTimer.check() && Action == MOVE)
    {
      State = ChooseBasket;
      StateTimer.reset();
    }
    pot_reading = new_pot_reading;
    break;
  }

  case ChooseBasket:
  {
    if (Basket == GOOD)
    {
      rotateAlphaGood();
      if (StateTimer.check())
      {
        State = HeadingToGoodBasket;
      } // Stops rotation after a duration.
    }
    else
    {
      rotateAlphaBad();
      if (StateTimer.check())
      {
        State = HeadingToBadBasket;
      } // Stops rotation after a duration.
    }
    break;
  }

  case HeadingToBadBasket:
  {
    MoveForward();
    if ((detectLine(lineLeftPin_in) == RED) || (detectLine(lineRightPin_in) == RED))
    {
      State = FollowingRedTapeToBasket;
    }
    break;
  }

  case HeadingToGoodBasket:
  {
    MoveForward();
    if ((detectLine(lineLeftPin_in) == RED) || (detectLine(lineRightPin_in) == RED))
    {
      State = IgnoreRedTapeToBasket;
      StateTimer.reset();
    }
    break;
  }

  case FollowingRedTapeToBasket:
  {
    followRedLine();
    if ((detectLine(lineLeftPin_in) == BLACK) || (detectLine(lineRightPin_in) == BLACK))
    {
      State = DumpingBalls;
      StateTimer.reset();
    }
    break;
  }

  case IgnoreRedTapeToBasket:
  {
    if (StateTimer.check())
    {
      if ((detectLine(lineLeftPin_in) == RED) || (detectLine(lineRightPin_in) == RED))
      {
        State = FollowingRedTapeToBasket;
      }
    }
    break;
  }

  case IgnoreRedTapeToStudio:
  {
    if (StateTimer.check())
    {
      if ((detectLine(lineLeftPin_in) == RED) || (detectLine(lineRightPin_in) == RED))
      {
        State = FollowingRedTapeToStudio;
      }
    }
    break;
  }

  case DumpingBalls:
  {
    StopMotors();
    RaiseGate();

    if (StateTimer.check())
    {
      LowerGate();
      StateTimer.reset();

      if (Basket == BAD)
      {
        rotateBetaBad();
        State = HeadingBackFromBadBasket;
      }
      else
      {
        rotateBetaGood();
        State = HeadingBackFromGoodBasket;
      }
    }
    break;
  }

  case HeadingBackFromBadBasket:
  {
    if (StateTimer.check())
    {
      MoveForward();
      if ((detectLine(lineLeftPin_in) == RED) || (detectLine(lineRightPin_in) == RED))
      {
        followRedLine();
        State = FollowingRedTapeToStudio;
      }
    }
    break;
  }

  case HeadingBackFromGoodBasket:
  {
    if (StateTimer.check())
    {
      MoveForward();
      if ((detectLine(lineLeftPin_in) == RED) || (detectLine(lineRightPin_in) == RED))
      {
        State = IgnoreRedTapeToStudio;
        StateTimer.reset();
      }
    }
    break;
  }

  case FollowingRedTapeToStudio:
  {
    int beacon_val = analogRead(beaconPin_in);
    if (beaconStrongEnough(beacon_val))
    {
      StopMotors();
      State = AtStudioNotOriented;
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

void LEDTimerHandler()
{
  ExecuteLEDandPotSM();

  if (LED_state == HIGH)
  {
    LED_state = LOW;
    analogWrite(LEDPin_out, map(pot_reading, 0, 1023, 0, 255));
  }
  else
  {
    LED_state = HIGH;
    analogWrite(LEDPin_out, 0);
  }
}

void ExecuteLEDandPotSM()
{
  // pot_reading = map(analogRead(potPin_in), 0, 1023, 0, 255);
  pot_reading = analogRead(potPin_in); // [in range 0, 1023]
  // PrintVar("Pot", pot_reading);
  if (inRange(pot_reading, GOOD_POT_CUTOFF, 1023))
  {
    // Serial.println("C1");
    Basket = GOOD;
    Action = MOVE;
    LED_freq = 10; // [Hz]
  }
  else if (inRange(pot_reading, 512, GOOD_POT_CUTOFF))
  {
    // Serial.println("C2");
    Basket = GOOD;
    Action = WAIT;
    LED_freq = 50; // [Hz]
  }
  else if (inRange(pot_reading, BAD_POT_CUTOFF, 512))
  {
    // Serial.println("C3");
    Basket = BAD;
    Action = WAIT;
    LED_freq = 50; // [Hz]
  }
  else
  {
    // Serial.println("C4");
    Basket = BAD;
    Action = MOVE;
    LED_freq = 10; // [Hz]
  }
}

void setRGBcolor(uint8_t color[])
{
  digitalWrite(RGB_RED_PIN, color[0]);
  digitalWrite(RGB_GREEN_PIN, color[1]);
  digitalWrite(RGB_BLUE_PIN, color[2]);
}
