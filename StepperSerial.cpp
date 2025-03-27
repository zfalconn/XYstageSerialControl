#include <AccelStepper.h>

const int DIRECTION_TOWARDS_MOTOR = -1;
const unsigned long MAX_X_STEPS = 120000;
const unsigned long MAX_Y_STEPS = 122666;

int step = 5000;
long stepX = 0;
long stepY = 0;
// Microstepping: 1/4 MS1 = GND, MS2 = VIO;
// Thread pitch: 1.5 mm / rev

//// Define pins for TMC2208
const int dirPinX = 5;  // DIR pin
const int stepPinX = 2; // STEP pin

const int dirPinY = 6;
const int stepPinY = 3;

const int Limit_SwitchX = 10; // Pressed returns 0
const int Limit_SwitchY = 12; // Pressed returns 0

AccelStepper stepperX(AccelStepper::DRIVER, stepPinX, dirPinX);
AccelStepper stepperY(AccelStepper::DRIVER, stepPinY, dirPinY);

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars]; // temporary array for use when parsing

long xCoord = 0;
long yCoord = 0;

boolean newData = false;
boolean manual = true;
char startMarker = '<';
char endMarker = '>';
char rc;
boolean isReceiving = false;

byte index = 0;

void homeMotors();
void recvString();
void parseCoord();

void setup()
{
  Serial.begin(9600);
  pinMode(Limit_SwitchX, INPUT);
  pinMode(Limit_SwitchY, INPUT);

  delay(5);
  // Configure the stepper motor for higher speed
  stepperX.setMaxSpeed(2000); // Increase max speed (steps per second)
  stepperX.setAcceleration(500);

  stepperY.setMaxSpeed(2000); // Increase max speed (steps per second)
  stepperY.setAcceleration(500);

  // Start the homing process
  // Serial.println("Homing Motors...");
  // homeMotors();
  // Serial.println("Motors homed");
}

void loop()
{
  if (Serial.available() > 0 && manual == true)
  {
    // Read the incoming byte
    char data = Serial.read();
    if (data == 'w')
    {
      stepperX.move(step);
      // Serial.println("X move +");
      stepX += step;
    }

    else if (data == 's' && digitalRead(Limit_SwitchX))
    {
      stepperX.move(-step);
      // Serial.println("X move -");
      stepX -= step;
    }
    else if (data == 'a')
    {
      stepperY.move(step);
      // Serial.println("Y move +");
      stepY += step;
    }
    else if (data == 'd' && digitalRead(Limit_SwitchY))
    {
      stepperY.move(-step);
      // Serial.println("Y move -");
      stepY -= step;
    }
    else if (data == '0')
    {
      step = 5000; // Reset step value
      // Serial.println("Reset step to 5000");
    }
    else if (data == '1')
    {
      step = 1;
      // Serial.println("Set step to 1");
    }
    else if (data == '2')
    {
      step = 10;
      // Serial.println("Set step to 10");
    }
    else if (data == '3')
    {
      step = 100;
      // Serial.println("Set step to 100");
    }
    else if (data == '4')
    {
      step = 1000;
      // Serial.println("Set step to 1000");
    }
    else if (data == 'h')
    {
      homeMotors();
      stepX = 0;
      stepY = 0;
    }
    else if (data == 'c')
    {
      // Serial.println("Entering Coordinates Mode.");
      manual = false;
    }
    else if (data == 'g')
    {
      String string_X = String(stepX);
      String string_Y = String(stepY);
      String srl_out = "X: " + string_X + ",Y: " + string_Y;
      Serial.println(srl_out);
    }
  }

  if (manual == false)
  {
    while (Serial.available() > 0 && newData == false)
    {
      rc = Serial.read();

      if (isReceiving == true)
      {
        if (rc != endMarker) // Collect char while not hitting end marker
        {
          receivedChars[index] = rc;
          index++;
          if (index >= numChars)
          {
            index = numChars - 1;
          }
        }
        else
        {
          receivedChars[index] = '\0'; // terminate the string
          isReceiving = false;         // reset status
          index = 0;
          newData = true; // reset status
        }
      }

      else if (rc == startMarker) // Begin receiving
      {
        isReceiving = true;
      }
    }

    if (newData == true)
    {
      strcpy(tempChars, receivedChars);
      parseCoord();
      stepperX.move(xCoord);
      stepperY.move(yCoord);
      stepX += xCoord;
      stepY += yCoord;
      newData = false;
      // reset back to manual mode
      manual = true;
    }
  }

  stepperX.run();
  stepperY.run();
}

void homeMotors()
{
  while (!digitalRead(Limit_SwitchX) || !digitalRead(Limit_SwitchY))
  {
    if (!digitalRead(Limit_SwitchX))
    {
      stepperX.move(5000);
    }
    if (!digitalRead(Limit_SwitchY))
    {
      stepperY.move(5000);
    }
    stepperX.run();
    stepperY.run();
  }

  stepperX.setSpeed(2000);
  stepperY.setSpeed(2000);
  // While we haven't hit the limit switch, keep moving
  while (digitalRead(Limit_SwitchX) || digitalRead(Limit_SwitchY))
  {
    if (digitalRead(Limit_SwitchX))
    {
      stepperX.move(DIRECTION_TOWARDS_MOTOR * 1000);
    }
    if (digitalRead(Limit_SwitchY))
    {
      stepperY.move(DIRECTION_TOWARDS_MOTOR * 1000);
    }
    stepperX.run();
    stepperY.run();
  }
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
}

void recvString()
{
  // static boolean isReceiving = false;
  // static byte index = 0;
  // char startMarker = '<';
  // char endMarker = '>';
  // char rc;

  while (Serial.available() > 0 && newData == false)
  {
    rc = Serial.read();

    if (isReceiving == true)
    {
      if (rc != endMarker) // Collect char while not hitting end marker
      {
        receivedChars[index] = rc;
        index++;
        if (index >= numChars)
        {
          index = numChars - 1;
        }
      }
      else
      {
        receivedChars[index] = '\0'; // terminate the string
        isReceiving = false;         // reset status
        index = 0;
        newData = true; // reset status
      }
    }

    else if (rc == startMarker) // Begin receiving
    {
      isReceiving = true;
    }
  }
}

void parseCoord()
{
  char *strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ","); // get the first part - the string
  xCoord = atol(strtokIndx);           // convert string to X long int

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  yCoord = atol(strtokIndx);      // convert string to Y long int
}
