#include <FABRIK2D.h>
#include <Adafruit_PWMServoDriver.h>

int lengths[] = {194, 150};
Fabrik2D fabrik2D(3,lengths);

int prevTiltAngle, prevElbAngle;
int x, y;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 140
#define SERVOMAX 520

uint16_t tilt_deg(int degree) {
  uint16_t pulse = map(degree, 0, 180, SERVOMIN, SERVOMAX);

  return pulse;
}

uint16_t elbow_deg(int degree) {
  const int MIN = 200;
  const int MAX = 500;

  uint16_t converted = map(degree, -180, 0, 0, 180);
  uint16_t pulse = map(converted, 0, 180, MIN, MAX);

  return pulse;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Ready!");

  pwm.begin();
  pwm.setPWMFreq(60);

  fabrik2D.setTolerance(0.5);

  // initialize robot when start to go to x=0 and y=0;
  pwm.setPWM(0, 0, tilt_deg(90));
  pwm.setPWM(1, 0, elbow_deg(-180));
  
  delay(2000);
  
}

void loop() {
  x+=5;
  y+=10;
  // the x, y coordinates of where the arm should move next;
  fabrik2D.solve(x,y,lengths);

  Serial.print("x is ");
  Serial.println(x);

  Serial.print("y is ");
  Serial.println(y);

  // conversion from radians to degrees
  int tiltAngle = fabrik2D.getAngle(0) * 57296 / 1000;
  int elbowAngle = fabrik2D.getAngle(1) * 57296 / 1000;

  Serial.println(tiltAngle);
  Serial.println(elbowAngle);

  delay(2000);
  // TODO convert the negative angles first before processing -- add another function to do the work(?)
  for (int i=0; i < (max(tiltAngle, elbowAngle)); i++) {
    if ((tiltAngle - prevTiltAngle) > 0) {
      if (prevTiltAngle != tiltAngle) {
        prevTiltAngle++;
        pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
        Serial.print("Iterate tilt angle ");
        Serial.println(prevTiltAngle, tiltAngle);
      }
    } else if ((tiltAngle - prevTiltAngle) < 0) {
        if (prevTiltAngle != tiltAngle) {
          prevTiltAngle--;
          pwm.setPWM(0, 0, tilt_deg(prevTiltAngle));
          Serial.print("Iterate tilt angle ");
          Serial.println(prevTiltAngle, tiltAngle);
        }
    }

    if ((elbowAngle - prevTiltAngle) > 0) {
      if (prevElbAngle != elbowAngle) {
        prevElbAngle++;
        pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
        Serial.print("Iterate elbow angle ");
        Serial.println(prevElbAngle, elbowAngle);
      }
    } else if ((elbowAngle - prevTiltAngle) < 0) {
        if (prevElbAngle != elbowAngle) {
          prevElbAngle--;
          pwm.setPWM(1, 0, elbow_deg(prevElbAngle));
          Serial.print("Iterate elbow angle ");
          Serial.println(prevElbAngle, elbowAngle);
        }
    }

    if (prevTiltAngle == tiltAngle && prevElbAngle == elbowAngle) {
      break;
    }
  }

  // update next angle value for next movement;
  prevTiltAngle = tiltAngle;
  prevElbAngle = elbowAngle;
  
  Serial.print("Previous is tilt ");
  Serial.println(prevTiltAngle);

  Serial.print("Previous is elbow ");
  Serial.println(prevElbAngle);

  delay(3000);
}
