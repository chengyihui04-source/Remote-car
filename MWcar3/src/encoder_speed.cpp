#include <Arduino.h>
#include "encoder_speed.h"

#define ENCODER_FL_A 34
#define ENCODER_FL_B 35
#define ENCODER_RL_A 32
#define ENCODER_RL_B 31
#define ENCODER_FR_A 22
#define ENCODER_FR_B 23
#define ENCODER_RR_A 15
#define ENCODER_RR_B 2

#define ENCODER_COUNT_PER_ROUND 4680.0f
#define SPEED_SAMPLE_INTERVAL_MS 500UL

volatile long encoderCountFL = 0;
volatile long encoderCountRL = 0;
volatile long encoderCountFR = 0;
volatile long encoderCountRR = 0;

long lastCountFL = 0;
long lastCountRL = 0;
long lastCountFR = 0;
long lastCountRR = 0;

float rpmFL = 0.0f;
float rpmRL = 0.0f;
float rpmFR = 0.0f;
float rpmRR = 0.0f;

unsigned long lastSampleTime = 0;

void IRAM_ATTR onEncoderFL() {
  encoderCountFL += digitalRead(ENCODER_FL_B) ? 1 : -1;
}

void IRAM_ATTR onEncoderRL() {
  encoderCountRL += digitalRead(ENCODER_RL_B) ? 1 : -1;
}

void IRAM_ATTR onEncoderFR() {
  encoderCountFR += digitalRead(ENCODER_FR_B) ? 1 : -1;
}

void IRAM_ATTR onEncoderRR() {
  encoderCountRR += digitalRead(ENCODER_RR_B) ? 1 : -1;
}

void encoderSpeedInit() {
  pinMode(ENCODER_FL_A, INPUT_PULLUP);
  pinMode(ENCODER_FL_B, INPUT_PULLUP);
  pinMode(ENCODER_RL_A, INPUT_PULLUP);
  pinMode(ENCODER_RL_B, INPUT_PULLUP);
  pinMode(ENCODER_FR_A, INPUT_PULLUP);
  pinMode(ENCODER_FR_B, INPUT_PULLUP);
  pinMode(ENCODER_RR_A, INPUT_PULLUP);
  pinMode(ENCODER_RR_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_FL_A), onEncoderFL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_RL_A), onEncoderRL, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_FR_A), onEncoderFR, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_RR_A), onEncoderRR, RISING);

  lastSampleTime = millis();
}

void encoderSpeedUpdate() {
  unsigned long now = millis();
  unsigned long elapsed = now - lastSampleTime;

  if (elapsed < SPEED_SAMPLE_INTERVAL_MS) {
    return;
  }

  noInterrupts();
  long countFL = encoderCountFL;
  long countRL = encoderCountRL;
  long countFR = encoderCountFR;
  long countRR = encoderCountRR;
  interrupts();

  float dt = elapsed / 1000.0f;

  rpmFL = ((countFL - lastCountFL) / dt) * 60.0f / ENCODER_COUNT_PER_ROUND;
  rpmRL = ((countRL - lastCountRL) / dt) * 60.0f / ENCODER_COUNT_PER_ROUND;
  rpmFR = ((countFR - lastCountFR) / dt) * 60.0f / ENCODER_COUNT_PER_ROUND;
  rpmRR = ((countRR - lastCountRR) / dt) * 60.0f / ENCODER_COUNT_PER_ROUND;

  lastCountFL = countFL;
  lastCountRL = countRL;
  lastCountFR = countFR;
  lastCountRR = countRR;
  lastSampleTime = now;

  Serial.print("rpm FL=");
  Serial.print(rpmFL, 2);
  Serial.print(" RL=");
  Serial.print(rpmRL, 2);
  Serial.print(" FR=");
  Serial.print(rpmFR, 2);
  Serial.print(" RR=");
  Serial.println(rpmRR, 2);
}

float getRpmFL() {
  return rpmFL;
}

float getRpmRL() {
  return rpmRL;
}

float getRpmFR() {
  return rpmFR;
}

float getRpmRR() {
  return rpmRR;
}
