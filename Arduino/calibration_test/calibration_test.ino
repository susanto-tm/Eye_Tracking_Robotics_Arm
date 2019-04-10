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
  
  pwm.setPWM(0, 0, tilt_deg(90));
  pwm.setPWM(1, 0, elbow_deg(-90));
  pwm.setPWM(1, 0, elbow_deg(-20));
  pwm.setPWM(4, 0, gripper_deg(180));

  fabrik2D.setTolerance(0.5);

  delay(1000);

}

int grip_state = 0;

void loop() {
  
  // put your main code here, to run repeatedly:
//  fabrik2D.setJoints(angles, lengths);
//  fabrik2D.setBaseAngle(3.14);
//
//  int xCoord = fabrik2D.getX(3);
//  int yCoord = fabrik2D.getY(3);
//  int zCoord = fabrik2D.getZ();
//
//  Serial.print("X is: ");
//  Serial.println(xCoord);
//
//  Serial.print("Y is: ");
//  Serial.println(yCoord);
//
//  Serial.print("Z is: ");
//  Serial.println(zCoord);
//  
//  pwm.setPWM(0, 0, tilt_deg(90));
//  pwm.setPWM(1, 0, elbow_deg(90));
//  pwm.setPWM(2, 0, wrist_deg(0));
//
//  delay(5000);

//if (x != 90) {
//  x++;
//}
//if (y != -90) {
//  y--;
//}
//if (z != -90) {
//  z--;
//}
//pwm.setPWM(0, 0, tilt_deg(90));
//pwm.setPWM(1, 0, elbow_deg(-90));
//pwm.setPWM(2, 0, wrist_deg(180));

//  fabrik2D.setBaseAngle(3.14);
//
//  int zCoordB = fabrik2D.getZ();
//  Serial.println(zCoordB);
//
//  delay(5000);
  
//  fabrik2D.solve2(200, 20, 0, lengths);
//
//  int tiltAngle = fabrik2D.getAngle(0) * 57296/1000;
//  int elbowAngle = fabrik2D.getAngle(1) * 57296/1000;
//  int wristAngle = fabrik2D.getAngle(2) * -57296/1000;
//  int baseAngle = fabrik2D.getBaseAngle() * 57296/1000;
//
//  pwm.setPWM(0, 0, tilt_deg(tiltAngle));
//  pwm.setPWM(1, 0, elbow_deg(elbowAngle));
//  pwm.setPWM(2, 0, wrist_deg(wristAngle));
//  pwm.setPWM(3, 0, base_deg(90));
//
//  int xCoord = fabrik2D.getX(3);
//  int yCoord = fabrik2D.getY(3);
//  int zCoord = fabrik2D.getZ();
//
//  Serial.print("Tilt Angle: ");
//  Serial.println(tiltAngle);
//
//  Serial.print("Elbow Angle: ");
//  Serial.println(elbowAngle);
//
//  Serial.print("Wrist Angle: ");
//  Serial.println(wristAngle);
//
//  Serial.print("Base Angle: ");
//  Serial.println(baseAngle);
//
//  Serial.print("Check X: ");
//  Serial.println(xCoord);
//
//  Serial.print("Check Y: ");
//  Serial.println(yCoord);
//
//  Serial.print("Check Z: ");
//  Serial.println(zCoord);
//  if (grip_state == 0) {
//    pwm.setPWM(0, 0, tilt_deg(70));
//    delay(200);
//    pwm.setPWM(1, 0, elbow_deg(-45));
//    delay(200);
//    pwm.setPWM(2, 0, wrist_deg(35));
//    delay(200);
//    pwm.setPWM(3, 0, base_deg(90));
//    delay(1000);
//
//    grip_state = 1;
//  }
//
//  else if (grip_state == 1) {
//    pwm.setPWM(4, 0, gripper_deg(0));
//    grip_state = 2;
//    delay(2000);
//  }
//  else if (grip_state == 2) {
//    pwm.setPWM(1, 0, elbow_deg(-20));
//    delay(2000);
//    pwm.setPWM(0, 0, tilt_deg(80));
//    pwm.setPWM(1, 0, elbow_deg(-40));
//    delay(2000);
//    pwm.setPWM(4, 0, gripper_deg(180));
//    delay(2000);
//
//    grip_state = 3;
//  }
//  else if (grip_state == 3) {
//    pwm.setPWM(4, 0, gripper_deg(0));
//    delay(2000);
//    grip_state = 2;
//  }

//  pwm.setPWM(0, 0, tilt_deg(100));
//  pwm.setPWM(1, 0, elbow_deg(-80));
//  pwm.setPWM(2, 0, wrist_deg(45));
//  pwm.setPWM(3, 0, base_deg(90));
//  pwm.setPWM(4, 0, gripper_deg(90));

  fabrik2D.solve2(150, 50, 30, -M_PI/4, lengths);

  Serial.println("Z at 0");

  int tAngle = fabrik2D.getAngle(0) * 57296/1000;
  int eAngle = fabrik2D.getAngle(1) * 57296/1000;
  int wAngle = fabrik2D.getAngle(2) * -57296/1000;
  int bAngle = fabrik2D.getBaseAngle()* 57296/1000;

  int xC = fabrik2D.getX(3);
  int yC = fabrik2D.getY(3);
  int zC = fabrik2D.getZ();

  Serial.print("X is: ");
  Serial.println(xC);

  Serial.print("Y is: ");
  Serial.println(yC);

  Serial.print("Z is: ");
  Serial.println(zC);

  pwm.setPWM(0, 0, tilt_deg(tAngle));
  pwm.setPWM(1, 0, elbow_deg(eAngle));
  pwm.setPWM(2, 0, wrist_deg(wAngle));
  pwm.setPWM(3, 0, base_deg(bAngle));

  delay(2000);

  fabrik2D.solve2(200, 150, 30, -M_PI/4, lengths);

  Serial.println("Z at 50");

  tAngle = fabrik2D.getAngle(0) * 57296/1000;
  eAngle = fabrik2D.getAngle(1) * 57296/1000;
  wAngle = fabrik2D.getAngle(2) * -57296/1000;
  bAngle = fabrik2D.getBaseAngle() * 57296/1000;

  xC = fabrik2D.getX(3);
  yC = fabrik2D.getY(3);
  zC = fabrik2D.getZ();

  Serial.print("X is: ");
  Serial.println(xC);

  Serial.print("Y is: ");
  Serial.println(yC);

  Serial.print("Z is: ");
  Serial.println(zC);

  pwm.setPWM(0, 0, tilt_deg(tAngle));
  pwm.setPWM(1, 0, elbow_deg(eAngle));
  pwm.setPWM(2, 0, wrist_deg(wAngle));
  pwm.setPWM(3, 0, base_deg(bAngle));

  delay(2000);
//  pwm.setPWM(0, 0, tilt_deg(90));
//  pwm.setPWM(2, 0, wrist_deg(0));
  
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
