#include <FABRIK2D.h>
#include <Adafruit_PWMServoDriver.h>

int lengths[] = {194, 150};
Fabrik2D fabrik2D(3, lengths);

int prevTiltAngle, prevElbAngle;
int x, y;
int state = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 140
#define SERVOMAX 520

// Convert degrees to pulse width modulation for the tilt angle
uint16_t tilt_deg(int degree) {
  uint16_t pulse = map(degree, 0, 180, SERVOMIN, SERVOMAX);

  return pulse;
}

// Convert degrees to pulse width modulation for the elbow angle
uint16_t elbow_deg(int degree) {
  const int MIN = 200;
  const int MAX = 500;

  uint16_t pulse = map(degree, 0, 180, MIN, MAX);

  return pulse;
}

// Pass by reference function to calculate initialized position and return initialized angles
void initializeStart(int &xCoord, int &yCoord) {
  xCoord = 0;
  yCoord = 0;

  fabrik2D.solve(xCoord, yCoord, lengths);

  int tAngle = fabrik2D.getAngle(0) * 57296/1000;
  int eAngle = fabrik2D.getAngle(1) * 57296/1000;

  pwm.setPWM(0, 0, tilt_deg(tAngle));
  pwm.setPWM(1, 0, elbow_deg(eAngle));
  Serial.println("Initialized");

  xCoord = tAngle;
  yCoord = eAngle;
}

uint16_t negativeConvert(int degree) {
  uint16_t converted = map(degree, -180, 0, 0, 180);

  return converted;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Ready!");

  pwm.begin();
  pwm.setPWMFreq(60);

  delay(2000);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // Runs only once to initialize the arm to (0, 0)
  if (state == 0) {
    prevTiltAngle = 0;
    prevElbAngle = 0;
    initializeStart(prevTiltAngle, prevElbAngle);
    state = 1;
    delay(1000);
  }
  
  x+=5;
  y+=10;

  Serial.print("x is: ");
  Serial.println(x);

  Serial.print("y is: ");
  Serial.println(y);

  fabrik2D.solve(x, y, lengths);

  // Get angles from the calculation and convert them into degrees from radians
  int tiltAngle = fabrik2D.getAngle(0) * 57296/1000;
  int negElbowAngle = fabrik2D.getAngle(1) * 57296/1000;
  
  int elbowAngle = negativeConvert(negElbowAngle);

  Serial.print("Tilt Angle is: ");
  Serial.println(tiltAngle);
  Serial.print("Elbow Angle is: ");
  Serial.println(elbowAngle);
  
  delay(1000);
  //Slow rotation of servos
  for (int i=0; i < (max(tiltAngle, elbowAngle)); i++) {
    if (((tiltAngle - prevTiltAngle) > 0) && prevTiltAngle != tiltAngle) {
      prevTiltAngle++;
      pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
    }
    else if (((tiltAngle - prevTiltAngle) < 0) && prevTiltAngle != tiltAngle) {
      prevTiltAngle--;
      pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
    }

    if (((elbowAngle - prevElbAngle) > 0) && prevElbAngle != elbowAngle) {
      prevElbAngle++;
      pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
    }
    else if (((elbowAngle - prevElbAngle) < 0) && prevElbAngle != elbowAngle) {
      prevElbAngle--;
      pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
    }

    if (prevTiltAngle == tiltAngle && prevElbAngle == elbowAngle) {
      break;
    }
  }

  prevTiltAngle = tiltAngle;
  prevElbAngle = elbowAngle;

  delay(2000);
  
}
