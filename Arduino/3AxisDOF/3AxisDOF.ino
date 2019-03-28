#include <FABRIK2D.h>
#include <Adafruit_PWMServoDriver.h>

// define main constant variables for servo PWM
#define SERVOMIN 140
#define SERVOMAX 520

// initialize length of joins for arm
int lengths[] = {100, 130, 180};
Fabrik2D fabrik2D(4, lengths);

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

  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  receiveStartEndMarker();
  if (newData == true) {
    strcpy(tempChars, receivedChars); // creates a copy of receivedChars since strtok() will
                                      // replace commas with \0
    parseData();
    showData();
    newData = false; // process new coordinates
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

uint16_t base_deg(int degree) {
  uint16_t pulse = map(degree, 0, 180, SERVOMIN, SERVOMAX);

  return pulse;
}
