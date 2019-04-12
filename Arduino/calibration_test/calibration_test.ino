#include <FABRIK2D.h>
#include <Adafruit_PWMServoDriver.h>

#define SERVOMIN 130
#define SERVOMAX 510

int lengths[] = {100, 130, 180};
int angles[] = {2, 2, 0};
int anglesDegrees[] = {120, 90, 0};
Fabrik2D fabrik2D(4, lengths);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int x = 0;
int y = 20;
int z = 0;

int state = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Ready");

  pwm.begin();
  pwm.setPWMFreq(60);
  
  pwm.setPWM(4, 0, gripper_deg(180));

  fabrik2D.setTolerance(0.5);

  delay(1000);

}

int grip_state = 0;

void loop() {
  int tAngle, eAngle, wAngle, bAngle;
  
  // put your main code here, to run repeatedly:
  if (grip_state == 0) {
    fabrik2D.solve2(150, 170, 200, lengths);
  
    tAngle = fabrik2D.getAngle(0) * 57926/1000;
    eAngle = fabrik2D.getAngle(1) * 57296/1000;
    wAngle = fabrik2D.getAngle(2) * -57296/1000;
    bAngle = fabrik2D.getBaseAngle();
  
    pwm.setPWM(0, 0, tilt_deg(tAngle));
    pwm.setPWM(1, 0, elbow_deg(eAngle));
    pwm.setPWM(2, 0, wrist_deg(wAngle));
    pwm.setPWM(3, 0, base_deg(90));

    grip_state = 0;
    delay(2000);
  }
  
  else if (grip_state == 1) {
    pwm.setPWM(0, 0, tilt_deg(100));
    delay(200);
    pwm.setPWM(1, 0, elbow_deg(-51));
    delay(200);
    pwm.setPWM(2, 0, wrist_deg(45));
    delay(200);
    pwm.setPWM(3, 0, base_deg(90));
    delay(1000);

    grip_state = 2;
  }

  else if (grip_state == 2) {
    pwm.setPWM(4, 0, gripper_deg(0));
    grip_state = 3;
    delay(2000);
  }
  else if (grip_state == 3) {
    pwm.setPWM(1, 0, elbow_deg(-20));
    delay(1000);
    pwm.setPWM(0, 0, tilt_deg(100));
    pwm.setPWM(1, 0, elbow_deg(-51));
    delay(2000);
    pwm.setPWM(4, 0, gripper_deg(180));
    delay(1000);
    grip_state = 4;
  }
  else if (grip_state == 4) {
    pwm.setPWM(4, 0, gripper_deg(0));
    delay(2000);
    grip_state = 3;
  }

  delay(1000);
  
}

void initialize() {
  pwm.setPWM(0, 0, tilt_deg(90));
  pwm.setPWM(1, 0, elbow_deg(-90));

  delay(1000);
}

void setArmAngles(int* angles) {
  
  
  pwm.setPWM(0, 0, tilt_deg(anglesDegrees[0]));
  pwm.setPWM(1, 0, elbow_deg(anglesDegrees[1]));
  pwm.setPWM(2, 0, wrist_deg(anglesDegrees[2]));
  // pwm.setPWM(3, 0, base_deg(angles[3]));
  
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
  const int MAX = 530;
  const int MIN = 130;

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
  uint16_t pulse = map(degree, 180, 0, MIN, MAX);

  return pulse;
  
}
