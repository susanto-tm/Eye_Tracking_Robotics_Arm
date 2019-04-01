#include <FABRIK2D.h>
#include <Adafruit_PWMServoDriver.h>

// define main constant variables for servo PWM
#define SERVOMIN 130
#define SERVOMAX 440

// calibrate coordinates for arm based on eye detection
const int digitalPin1 = 4;
const int digitalPin2 = 2;
int countConfirm = 0;
int calibrationConfirm = 0;
int calibrationReset = 0;

int calibrationState = 0;

// initialize length of joins for arm
int lengths[] = {100, 130, 180};
Fabrik2D fabrik2D(4, lengths);

// variables for inverse kinematics calculation
int state = 0;
int prevTiltAngle, prevElbAngle, prevWriAngle, prevBaseAngle;

// initialize PWMServoDriver library
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Receive byte string from python
const byte numChars = 12;
char receivedChars[numChars];
char tempChars[numChars];

// Store parsed data
int xData = 0;
int yData = 0;

// Indicate if coordinates are being processed
boolean newData = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Ready");

  pwm.begin();
  pwm.setPWMFreq(60);

  // digital input for button
  pinMode(digitalPin1, INPUT);
  pinMode(digitalPin2, INPUT);

  delay(5000);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (calibrationState == 0) {
    Serial.println("Starting Calibration...");
    receiveStartEndMarker();
    if (newData == true) {
      strcpy(tempChars, receivedChars);
      parseData();
      showData();
      calibrateCoordinates();
      newData = false;
    }
  }
  if (state == 1) {
    prevTiltAngle = 0;
    prevElbAngle = 0;
    prevWriAngle = 0;
    prevBaseAngle = 0;

    initializeStart(prevTiltAngle, prevElbAngle, prevWriAngle, prevBaseAngle);
    Serial.println("Initialized");

    state = 1;
    calibrationState = 2;
    delay(2000);
  }
  if (calibrationState == 2) {
   receiveStartEndMarker();
   if (newData == true) {
    strcpy(tempChars, receivedChars); // creates a copy of receivedChars since strtok() will
                                      // replace commas with \0
    parseData(); // parse incoming bytes data as coordinates
    showData();
    moveArm(); // calculate necessary inverse kinematics
    newData = false; // process new coordinates
    } 
  }
}

void calibrateCoordinates() {
    calibrationConfirm = digitalRead(digitalPin1);
    calibrationReset = digitalRead(digitalPin2);

    while (true) {
      if (calibrationConfirm == LOW) {
        countConfirm++;
        delay(150);
        if (countConfirm == 1) {
          Serial.println("Look at right coordinate");
        }
        // next press will confirm the coordinates and prompt next coordinate
        else if (countConfirm == 2) {
          int xMin = xData;
          countConfirm++;
        }
        // save coordinates in previous confirm so it does not save the wrong coordinates in the wrong variable
        else if (countConfirm == 3) {
          Serial.println("Look at left coordinate");
        }
        else if (countConfirm == 4) {
          int xMax = xData;
          countConfirm++;
        }
        else if (countConfirm == 5) {
          Serial.println("Look at bottom coordinate");
        }
        else if (countConfirm == 6) {
          int yMin = yData;
          countConfirm++;
          
        }
        else if (countConfirm == 7) {
          Serial.println("Look at top coordinate");
        }
        else if (countConfirm == 8) {
          int yMax = yData;
          countConfirm++;
          
        }
        else if (countConfirm == 9) {
          Serial.println("Wait for calibration mapping...");
          calibrationState = 1;
          state = 1;
        }
      }
      // reset countConfirm to 0 to reset the 
      else if (calibrationReset == LOW) {
        countConfirm = 1;
        Serial.println("Resetting Serial...");
        delay(150);
      }
      // if button is not pressed, break the loop and get new coordinates
      else {
        break;
      }
   }
}

void receiveStartEndMarker() {
  static boolean receiveProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (receiveProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate string with null operator
        Serial.println(receivedChars);
        receiveProgress = false;
        ndx = 0;
        newData = true; // pass data to parser
      }
    }
    else if (rc == startMarker) {
      receiveProgress = true; // skips the startMarker when collecting bytes begins
    }
  }
}

void parseData() {
  // split the data into its parts
  char* strtokIndx; // initialize a pointer strtokIndx that points to a char (address)

  strtokIndx = strtok(tempChars, ","); // takes tempChars as strings then searches for the string before it reaches the delimiter (comma)
  xData = atoi(strtokIndx); // takes strings up to the delimiter and changes it into int for first coordinate split

  strtokIndx = strtok(NULL, ","); // continues from previous token and finds for next split until another delimiter if NULL, then return last piece of string
  yData = atoi(strtokIndx); // takes the continued string and changes it into int for second coordinate split
}

void showData() {
  Serial.print("X Coordinate: ");
  Serial.println(xData);

  Serial.print("Y Coordinate: ");
  Serial.println(yData);
}


uint16_t tilt_deg(int degree) {
  uint16_t pulse = map(degree, 0, 180, SERVOMIN, SERVOMAX);

  return pulse;
}

uint16_t elbow_deg(int degree) {
  const int MIN = 200;
  const int MAX = 500;

  uint16_t pulse = map(degree, 0, 180, MIN, MAX);

  return pulse;
}

uint16_t wrist_deg(int degree) {
  const int MIN = 200;
  const int MAX = 500;

  uint16_t pulse = map(degree, 0, 180, MIN, MAX);

  return pulse;
}

uint16_t base_deg(int degree) {
  const int MIN = 120;
  const int MAX = 560;
  uint16_t pulse = map(degree, 0, 180, MIN, MAX);

  return pulse;
}

void initializeStart(int &xCoord, int &yCoord, int &wCoord, int &zCoord) {
  xCoord = 0;
  yCoord = 0;
  zCoord = 0;

  // fabrik2D.solve2(xCoord, yCoord, zCoord, -M_PI/4.0, lengths);

  int tAngle = fabrik2D.getAngle(0) * 57296/1000;
  int eAngle = fabrik2D.getAngle(1) * 57296/1000;
  int wAngle = fabrik2D.getAngle(2) * 57296/1000;
  int bAngle = fabrik2D.getBaseAngle() * 57296/1000;

  pwm.setPWM(0, 0, tilt_deg(90));
  pwm.setPWM(1, 0, elbow_deg(0));
  pwm.setPWM(2, 0, wrist_deg(wAngle));
  pwm.setPWM(3, 0, base_deg(90));

  xCoord = tAngle;
  yCoord = eAngle;
  wCoord = wAngle;
  zCoord = bAngle;
}

void moveArm() {
  Serial.println("Coordinates received and processing...");
  int y = 5;
  
  fabrik2D.solve2(yData, y, xData, -M_PI/4.0, lengths);

  int tiltAngle = fabrik2D.getAngle(0) * 57296/1000;
  // int elbowAngle = fabrik2D.getAngle(1) * 57296/1000;
  int wristAngle = fabrik2D.getAngle(2) * 57296/1000;
  int baseAngle = fabrik2D.getBaseAngle() * 57926/1000;

  int elbowAngle = 0;
  
  Serial.print("Base Angle is: ");
  Serial.println(baseAngle);

  for (int i=0; i < max(max(tiltAngle, elbowAngle), max(wristAngle, baseAngle)); i++) {
    //Tilt Angle always positive
    // If target angle is greater than current angle the increase current angle
    if (tiltAngle > prevTiltAngle) {
      prevTiltAngle++;
      pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
      delay(10);
    }
    // else if the target angle is less than current angle then decrease current angle
    else if (tiltAngle < prevTiltAngle) {
      prevTiltAngle--;
      pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
      delay(10);
    }

    // Elbow Angle
    // If the target angle is greater than the current angle then increase current angle
    if (elbowAngle > prevElbAngle) {
      prevElbAngle++;
      pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
      delay(10);
    }
    // else if the target angle is less than the current angle then decrease current angle
    else if (elbowAngle < prevElbAngle) {
      prevElbAngle--;
      pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
      delay(10);
    }

    // Wrist Angle
    // If the target angle is greater than the current angle then increase current angle
    if (wristAngle > prevWriAngle) {
      prevWriAngle++;
      pwm.setPWM(2, 0, wrist_deg(prevWriAngle));
      delay(10);
    }
    // else if the target angle is less than the current angle then decrease current angle
    else if (wristAngle < prevWriAngle) {
      prevWriAngle--;
      pwm.setPWM(2, 0, wrist_deg(prevWriAngle));
      delay(10);
    }

    // Base Angle
    // If the target angle is greater than the current angle then increase current angle
    if (baseAngle > prevBaseAngle) {
      prevBaseAngle++;
      pwm.setPWM(3, 0, base_deg(prevBaseAngle));
      delay(10);
    }
    // else if the target angle is less than the current angle then decrease current angle
    else if (baseAngle < prevBaseAngle) {
      prevBaseAngle--;
      pwm.setPWM(3, 0, base_deg(prevBaseAngle));
      delay(10);
    }
  }

  prevTiltAngle = tiltAngle;
  prevElbAngle = elbowAngle;
  prevWriAngle = wristAngle;
  prevBaseAngle = baseAngle;
  
}
