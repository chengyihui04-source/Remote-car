#include <Arduino.h>
#include "motor.h"

// left1
#define PWMA1 19
#define AIN1 18
#define AIN2 5

// right1
#define PWMB1 4
#define BIN1 16
#define BIN2 17

// left2
#define PWMA2 26
#define AIN3 25
#define AIN4 33

// right2
#define PWMB2 12
#define BIN3 14
#define BIN4 27

void motorInit() {
  pinMode(PWMA1, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB1, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(PWMA2, OUTPUT);
  pinMode(AIN3, OUTPUT);
  pinMode(AIN4, OUTPUT);

  pinMode(PWMB2, OUTPUT);
  pinMode(BIN3, OUTPUT);
  pinMode(BIN4, OUTPUT);

  motorStopA1();
  motorStopB1();
  motorStopA2();
  motorStopB2();
}

// left1
void motorForwardA1(int speed) {
  speed = constrain(speed, 0, 255);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA1, speed);
}

void motorBackwardA1(int speed) {
  speed = constrain(speed, 0, 255);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  analogWrite(PWMA1, speed);
}

void motorStopA1() {
  analogWrite(PWMA1, 0);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
}

// right1
void motorForwardB1(int speed) {
  speed = constrain(speed, 0, 255);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB1, speed);
}

void motorBackwardB1(int speed) {
  speed = constrain(speed, 0, 255);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
  analogWrite(PWMB1, speed);
}

void motorStopB1() {
  analogWrite(PWMB1, 0);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

// left2
void motorForwardA2(int speed) {
  speed = constrain(speed, 0, 255);
  digitalWrite(AIN3, HIGH);
  digitalWrite(AIN4, LOW);
  analogWrite(PWMA2, speed);
}

void motorBackwardA2(int speed) {
  speed = constrain(speed, 0, 255);
  digitalWrite(AIN3, LOW);
  digitalWrite(AIN4, HIGH);
  analogWrite(PWMA2, speed);
}

void motorStopA2() {
  analogWrite(PWMA2, 0);
  digitalWrite(AIN3, LOW);
  digitalWrite(AIN4, LOW);
}

// right2
void motorForwardB2(int speed) {
  speed = constrain(speed, 0, 255);
  digitalWrite(BIN3, HIGH);
  digitalWrite(BIN4, LOW);
  analogWrite(PWMB2, speed);
}

void motorBackwardB2(int speed) {
  speed = constrain(speed, 0, 255);
  digitalWrite(BIN3, LOW);
  digitalWrite(BIN4, HIGH);
  analogWrite(PWMB2, speed);
}

void motorStopB2() {
  analogWrite(PWMB2, 0);
  digitalWrite(BIN3, LOW);
  digitalWrite(BIN4, LOW);
}