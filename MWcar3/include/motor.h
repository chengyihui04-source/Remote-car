#pragma once
#include <Arduino.h>

void motorInit();

void motorForwardA1(int speed);
void motorBackwardA1(int speed);
void motorStopA1();

void motorForwardB1(int speed);
void motorBackwardB1(int speed);
void motorStopB1();

void motorForwardA2(int speed);
void motorBackwardA2(int speed);
void motorStopA2();

void motorForwardB2(int speed);
void motorBackwardB2(int speed);
void motorStopB2();