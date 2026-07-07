#pragma once
#include <Arduino.h>

void encoderSpeedInit();
void encoderSpeedUpdate();

float getRpmFL();
float getRpmRL();
float getRpmFR();
float getRpmRR();
