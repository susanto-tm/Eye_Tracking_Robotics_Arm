#include <Adafruit_PWMServoDriver.h>

// define main constant variables for servo PWM
#define SERVOMIN 130
#define SERVOMAX 510

// Base Angle Constraints
int bMin = 5;
int bMax = 175;

// Shoulder Angle Constraints
int tMin = 120;
int tMax = 60;

// Elbow Angle Constraints
int eMin = -70;
int eMax = -30;

// Wrist Angle Constraints
int wMin = 60;
int wMax = 30;

// calibrate coordinates for arm based on eye detection
int xMin, xMax, yMin, yMax;
int calibrationState = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// variables for inverse kinematics calculation
int state = 0;
int prevTiltAngle, prevElbAngle, prevWriAngle, prevBaseAngle;

// variables for gripping object
int grip_state = 0;
int grip_count = 0;

// Receive byte string from python
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];

// Store parsed data
int xData = 0;
int yData = 0;

// Indicate if coordinates are being processed
boolean newData = false;


// Reset during calibrationState = 2
void resetCalibrationStates() {
  pwm.setPWM(3, 0, base_deg(90));
  state = 1;
  calibrationState = 1;
  newData = false;
  
  Serial.println("Calibration reset complete...");
  delay(1000);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Ready");

  pwm.begin();
  pwm.setPWMFreq(60);

  delay(2000);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (state == 0) {
    prevTiltAngle = 120;
    prevElbAngle = -70;
    prevWriAngle = 60;
    prevBaseAngle = 90;

    pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
    pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
    pwm.setPWM(2, 0, wrist_deg(prevWriAngle));
    pwm.setPWM(3, 0, base_deg(prevBaseAngle));

    Serial.println("Initialized");

    delay(5000);
    
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
    // showData();
    moveArm(); // calculate necessary inverse kinematics
    newData = false; // process new coordinates
    } 
  }

}

uint16_t tilt_deg(int degree) {
  uint16_t pulse = map(degree, 0, 180, SERVOMIN, SERVOMAX);

  return pulse;
}

uint16_t elbow_deg(int degree) {
  const int MIN = 200;
  const int MAX = 550;

  uint16_t pulse = map(degree, 0, 180, MIN, MAX);

  return pulse;
}

uint16_t wrist_deg(int degree) {
  const int MIN = 130;
  const int MAX = 530;

  uint16_t pulse = map(degree, 0, 180, MIN, MAX);

  return pulse;
}

uint16_t base_deg(int degree) {
  const int MIN = 160;
  const int MAX = 480;
  uint16_t pulse = map(degree, 0, 180, MIN, MAX);

  return pulse;
}

uint16_t gripper_deg(int degree) {
  const int MIN = 200;
  const int MAX = 290;
  uint16_t pulse = map(degree, 180, 0, MIN, MAX); // 180 is open, 0 is close

  return pulse;
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
    else if (rc == 'G') {
      Serial.println("Running Gripper");
      gripperObjectPickup();
    }
    else if (rc == 'P') {
      Serial.println("Pausing or Running Arm");
    }
    else if (rc == 'R') {
      Serial.println("Resetting calibration states...");
      resetCalibrationStates();
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

void showData() {
  Serial.print("X Coordinate: ");
  Serial.println(xData);

  Serial.print("Y Coordinate: ");
  Serial.println(yData);
}

void moveArm() {
  int tiltAngle = map(yData, yMin, yMax, tMin, tMax);
  int elbowAngle = map(yData, yMin, yMax, eMin, eMax);
  int wristAngle = map(yData, yMin, yMax, wMin, wMax);
  int baseAngle = map(xData, xMin, xMax, bMin, bMax);

  for (int i=0; i < max(max(tiltAngle, elbowAngle), max(wristAngle, baseAngle)); i++) {
    // Tilt Angle
    if (tiltAngle > prevTiltAngle) {
      prevTiltAngle++;
      pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
    }
    else if (tiltAngle < prevTiltAngle) {
      prevTiltAngle--;
      pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
    }

    // Elbow Angle
    if (elbowAngle > prevElbAngle) {
      prevElbAngle++;
      pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
    }
    else if (elbowAngle < prevElbAngle) {
      prevElbAngle--;
      pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
    }

    // Wrist Angle
    if (wristAngle > prevWriAngle) {
      prevWriAngle++;
      pwm.setPWM(2, 0, wrist_deg(prevWriAngle));
    }
    else if (wristAngle < prevWriAngle) {
      prevWriAngle--;
      pwm.setPWM(2, 0, wrist_deg(prevWriAngle));
    }

    // Base Angle
    if (baseAngle > prevBaseAngle) {
      prevBaseAngle++;
      pwm.setPWM(3, 0, base_deg(prevBaseAngle));
    }
    else if (baseAngle < prevBaseAngle) {
      prevBaseAngle--;
      pwm.setPWM(3, 0, base_deg(prevBaseAngle));
    } 
  }

  prevTiltAngle = tiltAngle;
  prevElbAngle = elbowAngle;
  prevWriAngle = wristAngle;
  prevBaseAngle = baseAngle;
  
}

void gripperObjectPickup() {
  // If gripper grabs object
  if (grip_count == 0) {
    if (grip_state == 0) {
      pwm.setPWM(4, 0, gripper_deg(180)); // open gripper
      delay(200);
      pwm.setPWM(0, 0, tilt_deg(85));
      delay(200);
      pwm.setPWM(1, 0, elbow_deg(-45));
      delay(200);
      pwm.setPWM(2, 0, wrist_deg(45));
      delay(2000);

      grip_state = 1;
    }

    if (grip_state == 1) {
      pwm.setPWM(4, 0, gripper_deg(0)); // close gripper
      grip_state = 2;
      delay(2000);
    }

    Serial.println("Object picked up successful");
    
    prevTiltAngle = 85;
    prevElbAngle = -45;
    prevWriAngle = 45;

    grip_count = 1;
  }

  else if (grip_count == 1) {
    if (grip_state == 2) {
      pwm.setPWM(1, 0, elbow_deg(-55));
      delay(1000);
      pwm.setPWM(4, 0, gripper_deg(180)); // open gripper
      delay(1000);
      pwm.setPWM(1, 0, elbow_deg(-30));
      delay(2000);
    }

    Serial.println("Object dropped");

    pwm.setPWM(4, 0, gripper_deg(0)); // close gripper
    prevElbAngle = -30;
    grip_state = 0;
    grip_count = 0;
    
  }
}
