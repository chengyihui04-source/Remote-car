#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "motor.h"

struct RemoteData {
  int LX;
  int LY;
  int RX;
};

RemoteData latestData;
volatile bool hasNewData = false;

unsigned long lastRecvTime = 0;
bool alreadyStopped = false;

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);

// 你的摇杆中位大概 1800，所以 1600~2000 作为死区
#define JOY_LOW   1600
#define JOY_HIGH  2000

#define MAX_PWM 255
#define SPEED_DEAD 30

// ===================== 摇杆方向校准 =====================
// 你的左摇杆：
// 左推 LX 变大，右推 LX 变小。
// 但是下面这套麦轮公式里，x > 0 表示“向右平移”。
// 所以这里必须反向，让左推得到 x < 0，从而向左平移。
bool reverseX = true;

// 你的左摇杆：
// 上推 LY 变小，下推 LY 变大。
// 我们希望上推 y > 0，所以这里反向。
bool reverseY = true;

// 如果右摇杆 RX 和 LX 一样：左推数值变大、右推数值变小，
// 且我们希望左推原地左转，那么这里不要反向。
// 如果实车变成“左推右转”，只改这里为 true。
bool reverseTurn = false;

// ===================== 电机方向校准 =====================
// 你实测：左摇杆上推时，左前轮正常，另外三个轮子反了。
// 说明 B1、A2、B2 的电机正反方向和 A1 相反。
// 不用重新接线，在这里软件反向即可。
#define DIR_FL  1   // A1 左前：正常
#define DIR_FR -1   // B1 右前：反向
#define DIR_RL -1   // A2 左后：反向
#define DIR_RR -1   // B2 右后：反向

int joyToSpeed(int value, bool reverse) {
  int speed = 0;

  if (value >= JOY_LOW && value <= JOY_HIGH) {
    speed = 0;
  } else if (value > JOY_HIGH) {
    speed = map(value, JOY_HIGH, 4095, 0, MAX_PWM);
  } else {
    speed = map(value, JOY_LOW, 0, 0, -MAX_PWM);
  }

  speed = constrain(speed, -MAX_PWM, MAX_PWM);

  if (abs(speed) < SPEED_DEAD) {
    speed = 0;
  }

  if (reverse) {
    speed = -speed;
  }

  return speed;
}

bool isValidData(const RemoteData &d) {
  if (d.LX < 0 || d.LX > 4095) return false;
  if (d.LY < 0 || d.LY > 4095) return false;
  if (d.RX < 0 || d.RX > 4095) return false;
  return true;
}

void setMotorA1(int speed) {
  speed = constrain(speed, -255, 255);
  if (abs(speed) < SPEED_DEAD) speed = 0;

  if (speed > 0) {
    motorForwardA1(speed);
  } else if (speed < 0) {
    motorBackwardA1(-speed);
  } else {
    motorStopA1();
  }
}

void setMotorB1(int speed) {
  speed = constrain(speed, -255, 255);
  if (abs(speed) < SPEED_DEAD) speed = 0;

  if (speed > 0) {
    motorForwardB1(speed);
  } else if (speed < 0) {
    motorBackwardB1(-speed);
  } else {
    motorStopB1();
  }
}

void setMotorA2(int speed) {
  speed = constrain(speed, -255, 255);
  if (abs(speed) < SPEED_DEAD) speed = 0;

  if (speed > 0) {
    motorForwardA2(speed);
  } else if (speed < 0) {
    motorBackwardA2(-speed);
  } else {
    motorStopA2();
  }
}

void setMotorB2(int speed) {
  speed = constrain(speed, -255, 255);
  if (abs(speed) < SPEED_DEAD) speed = 0;

  if (speed > 0) {
    motorForwardB2(speed);
  } else if (speed < 0) {
    motorBackwardB2(-speed);
  } else {
    motorStopB2();
  }
}

void stopCar() {
  setMotorA1(0);
  setMotorB1(0);
  setMotorA2(0);
  setMotorB2(0);
}

void normalizeSpeeds(int &FL, int &FR, int &RL, int &RR) {
  int maxValue = max(max(abs(FL), abs(FR)), max(abs(RL), abs(RR)));

  if (maxValue > 255) {
    FL = FL * 255 / maxValue;
    FR = FR * 255 / maxValue;
    RL = RL * 255 / maxValue;
    RR = RR * 255 / maxValue;
  }
}

// 麦轮控制
// A1 = FL 左前轮
// B1 = FR 右前轮
// A2 = RL 左后轮
// B2 = RR 右后轮
void controlCar(RemoteData d) {
  int x = joyToSpeed(d.LX, reverseX);       // x > 0：向右平移，x < 0：向左平移
  int y = joyToSpeed(d.LY, reverseY);       // y > 0：前进
  int turn = joyToSpeed(d.RX, reverseTurn); // turn > 0：原地左转 / 逆时针

  // 标准麦轮混控：
  // 前进：四个轮子同向
  // 右平移：FL+ FR- RL- RR+
  // 左平移：FL- FR+ RL+ RR-
  // 左转：左侧轮子反转，右侧轮子正转
  int FL = y + x - turn;
  int FR = y - x + turn;
  int RL = y - x - turn;
  int RR = y + x + turn;

  normalizeSpeeds(FL, FR, RL, RR);

  // 先算出逻辑轮速，再根据你实测的电机方向进行修正
  int cmdFL = DIR_FL * FL;
  int cmdFR = DIR_FR * FR;
  int cmdRL = DIR_RL * RL;
  int cmdRR = DIR_RR * RR;

  setMotorA1(cmdFL);
  setMotorB1(cmdFR);
  setMotorA2(cmdRL);
  setMotorB2(cmdRR);

  Serial.print("LX=");
  Serial.print(d.LX);
  Serial.print(" LY=");
  Serial.print(d.LY);
  Serial.print(" RX=");
  Serial.print(d.RX);

  Serial.print(" | x=");
  Serial.print(x);
  Serial.print(" y=");
  Serial.print(y);
  Serial.print(" turn=");
  Serial.print(turn);

  Serial.print(" | logic FL=");
  Serial.print(FL);
  Serial.print(" FR=");
  Serial.print(FR);
  Serial.print(" RL=");
  Serial.print(RL);
  Serial.print(" RR=");
  Serial.print(RR);

  Serial.print(" | cmd A1=");
  Serial.print(cmdFL);
  Serial.print(" B1=");
  Serial.print(cmdFR);
  Serial.print(" A2=");
  Serial.print(cmdRL);
  Serial.print(" B2=");
  Serial.print(cmdRR);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  motorInit();
  stopCar();

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  Serial.println();
  Serial.println("receiver ready");
  Serial.print("receiver STA MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("esp now init failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  lastRecvTime = millis();
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  if (data_len != sizeof(RemoteData)) {
    return;
  }

  RemoteData tempData;
  memcpy(&tempData, data, sizeof(tempData));

  if (!isValidData(tempData)) {
    return;
  }

  latestData = tempData;
  hasNewData = true;
  lastRecvTime = millis();
  alreadyStopped = false;
}

void loop() {
  if (hasNewData) {
    hasNewData = false;
    controlCar(latestData);
  }

  // 500ms 没收到遥控数据，自动停车
  if (millis() - lastRecvTime > 500 && !alreadyStopped) {
    stopCar();
    alreadyStopped = true;
    Serial.println("signal lost, stop");
  }
}
