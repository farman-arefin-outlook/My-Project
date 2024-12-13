#include "AFMotor.h"
#include <Servo.h>

#define echopin A1 // echo pin
#define trigpin A0 // Trigger pin
#define ledPin A2

Servo myservo;

const int MOTOR_1 = 1; 
const int MOTOR_2 = 2; 
const int MOTOR_3 = 3; 
const int MOTOR_4 = 4; 

AF_DCMotor motor1(MOTOR_1, MOTOR12_64KHZ); // create motor object, 64KHz pwm
AF_DCMotor motor2(MOTOR_2, MOTOR12_64KHZ); // create motor object, 64KHz pwm
AF_DCMotor motor3(MOTOR_3, MOTOR12_64KHZ); // create motor object, 64KHz pwm
AF_DCMotor motor4(MOTOR_4, MOTOR12_64KHZ); // create motor object, 64KHz pwm

long previousMillis = 0;
long rotationDuration = 60000; // 1 minute in milliseconds
bool rotationComplete = false;

void setup() {
  Serial.begin(9600);           // Initialize serial port
  Serial.println("Start");

  myservo.attach(10);
  myservo.write(90);

  pinMode(ledPin, OUTPUT);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  
  motor1.setSpeed(180);          // set the motor speed to 0-255
  motor2.setSpeed(180);
  motor3.setSpeed(180);
  motor4.setSpeed(180);
}


void ambulanceLight() {
    for (int i = 0; i < 5; i++) {
        Serial.println("LED ON");
        digitalWrite(ledPin, HIGH);
        delay(100);
        Serial.println("LED OFF");
        digitalWrite(ledPin, LOW);
        delay(100);
    }
    delay(500);
}


void loop() {
  ambulanceLight();
  if (!rotationComplete) {
    rotate360();
  } else {
    obstacleAvoidance();
  }
}

void rotate360() {
  long currentMillis = millis();

  if (currentMillis - previousMillis <= rotationDuration) {
    Serial.println("Rotating...");
    motor1.run(FORWARD);   // Left wheels forward
    motor2.run(FORWARD);
    motor3.run(BACKWARD);  // Right wheels backward
    motor4.run(BACKWARD);
  } else {
    Serial.println("Rotation complete");
    motor1.run(RELEASE);   // Stop motors
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
    rotationComplete = true; // Mark rotation as complete
  }
}

void obstacleAvoidance() {
  int distance_F = data();
  Serial.print("S=");
  Serial.println(distance_F);
  if (distance_F > 20) {
    Serial.println("Forward");
    motor1.run(FORWARD);         // Move forward
    motor2.run(FORWARD); 
    motor3.run(FORWARD); 
    motor4.run(FORWARD);
  } else {
    hc_sr4(); // Perform obstacle avoidance
  }
}

long data() {
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  long distance = pulseIn(echopin, HIGH);
  return distance / 29 / 2;
}

void hc_sr4() {
  Serial.println("Stop");
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);

  ambulanceLight();
  
  myservo.write(0);
  delay(300);
  int distance_R = data();
  delay(100);
  myservo.write(170);
  delay(500);
  int distance_L = data();
  delay(100);
  myservo.write(90);
  delay(300);
  compareDistance(distance_L, distance_R);
}

void compareDistance(int distance_L, int distance_R) {
  if (distance_L > distance_R) {
    motor1.run(BACKWARD);   // Turn left
    motor2.run(BACKWARD);
    motor3.run(FORWARD); 
    motor4.run(FORWARD);
    delay(350);
  } else if (distance_R > distance_L) {
    motor1.run(FORWARD);    // Turn right
    motor2.run(FORWARD); 
    motor3.run(BACKWARD); 
    motor4.run(BACKWARD);
    delay(350);
  } else {
    motor1.run(BACKWARD);   // Move backward briefly
    motor2.run(BACKWARD);
    motor3.run(BACKWARD); 
    motor4.run(BACKWARD); 
    delay(300);
    motor1.run(BACKWARD);   // Turn left slightly
    motor2.run(BACKWARD);
    motor3.run(FORWARD); 
    motor4.run(FORWARD);
    delay(500);
  }
}
