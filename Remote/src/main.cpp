#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#define leftX 35
#define leftY 34
#define rightX 32

// 这里是告诉程序：TFT 每根线接到 ESP32 的哪个引脚
#define TFT_CS    21
#define TFT_DC    19
#define TFT_RST   17
#define TFT_MOSI  23
#define TFT_SCLK  18
#define TFT_BLK   22

uint8_t MAC_ADD[6] = {0x1C, 0xC3, 0xAB, 0xC3, 0xB5, 0xB8};

esp_now_peer_info_t car = {};

struct RemoteData {
    int LX;
    int LY;
    int RX;
};

RemoteData myData;

// 创建一个屏幕对象
Adafruit_ST7789 tft = Adafruit_ST7789(
  TFT_CS,
  TFT_DC,
  TFT_MOSI,
  TFT_SCLK,
  TFT_RST
);

void setup() {
    Serial.begin(115200);
    delay(1000);

    // 初始化屏幕
    // 如果你的屏是 240x240，就用 240, 240
    // 如果不对，后面再改成 240, 320 或 135, 240
    tft.init(240, 240);

    // 设置屏幕方向
    tft.setRotation(0);

    // 清屏，填充黑色
    tft.fillScreen(ST77XX_BLACK);

    // 设置文字颜色
    tft.setTextColor(ST77XX_WHITE);

    // 设置文字大小
    tft.setTextSize(2);

    // 设置光标位置
    tft.setCursor(20, 40);

    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);

    // 显示文字
    tft.println("Hello ESP32");

     tft.setCursor(20, 80);
    tft.println("ST7789 TFT");

    pinMode(leftX, INPUT);
    pinMode(leftY, INPUT);
    pinMode(rightX, INPUT);

    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        Serial.println("esp now init failed");
        return;
    }

    memcpy(car.peer_addr, MAC_ADD, 6);
    car.channel = 1;
    car.encrypt = false;
    car.ifidx = WIFI_IF_STA;

    esp_err_t addResult = esp_now_add_peer(&car);

    if (addResult == ESP_OK) {
        Serial.println("add peer ok");
    } else {
        Serial.print("add peer failed: ");
        Serial.println(addResult);
        return;
    }

    Serial.println("sender ready");
}

void loop() {
    myData.LX=analogRead(leftX);
    myData.LY=analogRead(leftY);
    myData.RX=analogRead(rightX);

    esp_err_t result = esp_now_send(car.peer_addr, (uint8_t *)&myData, sizeof(myData));

    if (result == ESP_OK) {
        Serial.println("send ok");
    } else {
        Serial.print("send failed: ");
        Serial.println(result);
    }

  delay(500);
}