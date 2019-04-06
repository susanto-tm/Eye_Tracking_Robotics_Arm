#include <FABRIK2D.h>
#include <Adafruit_PWMServoDriver.h>

// define main constant variables for servo PWM
#define SERVOMIN 130
#define SERVOMAX 440

// calibrate coordinates for arm based on eye detection
int xMin, xMax, yMin, yMax;
int calibrationState = 0;
int xMinCoord, xMaxCoord, yMinCoord, yMaxCoord;

// initialize length of joins for arm
int lengths[] = {100, 130, 180};
Fabrik2D fabrik2D(4, lengths);

// variables for inverse kinematics calculation
int state = 0;
int prevTiltAngle, prevElbAngle, prevWriAngle, prevBaseAngle;

// initialize PWMServoDriver library
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Receive byte string from python
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];

// Store parsed data
int xData = 0;
int yData = 0;

// Indicate if coordinates are being processed
boolean newData = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Ready");

  pwm.begin();
  pwm.setPWMFreq(60);

  delay(5000);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (state == 0) {
    prevTiltAngle = 0;
    prevElbAngle = 0;
    prevWriAngle = 0;
    prevBaseAngle = 0;

    initializeStart(prevTiltAngle, prevElbAngle, prevWriAngle, prevBaseAngle);
    Serial.println("Initialized");

    state = 1;
    calibrationState = 1;
    delay(2000);
  }
  else if (calibrationState == 1) {
    receiveStartEndMarker();
    if (newData == true) {
      strcpy(tempChars, receivedChars);
      parseCalibrationData();

      showCalibrationData();
      
      newData = false;
      calibrationState = 2;
    }
  }
  else if (calibrationState == 2) {
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

void showCalibrationData() {
  Serial.println("Calibration results");
  
  Serial.print("xMin is: ");
  Serial.println(xMin);

  Serial.print("xMax is: ");
  Serial.println(xMax);

  Serial.print("yMin is: ");
  Serial.println(yMin);

  Serial.print("yMax is: ");
  Serial.println(yMax);

  Serial.println("End");

  calibrationState = 2;
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

void parseCalibrationData() {
  char* strtokIndx;

  Serial.println(tempChars);

  strtokIndx = strtok(tempChars, ",");
  xMin = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  xMax = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  yMin = atoi(strtokIndx);

  Serial.println(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  yMax = atoi(strtokIndx);
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

  fabrik2D.solve2(xCoord, yCoord, zCoord, -M_PI/4.0, lengths);

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
  
  // Change xMinCoord and xMaxCoord **Z axis change on robot** to calibrated locations on the actual robot using forward kinematics to find them
  int xCalibrated = map(xData, xMin, xMax, xMinCoord, xMaxCoord);

  // Change yMinCoord and yMaxCoord **X axis change on robot** to calibrated locations on the actual robot using forward kinematics to find them
  int yCalibrated = map(yData, yMin, yMax, yMinCoord, yMaxCoord);
  
  fabrik2D.solve2(yCalibrated, y, xCalibrated, -M_PI/4.0, lengths);

  int tiltAngle = fabrik2D.getAngle(0) * 57296/1000;
  int elbowAngle = fabrik2D.getAngle(1) * 57296/1000;
  int wristAngle = fabrik2D.getAngle(2) * 57296/1000;
  int baseAngle = fabrik2D.getBaseAngle() * 57926/1000;
  
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
