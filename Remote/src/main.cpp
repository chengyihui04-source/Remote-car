#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#define leftX 35
#define leftY 34
#define rightX 32

uint8_t MAC_ADD[6] = {0x1C, 0xC3, 0xAB, 0xC3, 0xB5, 0xB8};

esp_now_peer_info_t car = {};

struct RemoteData {
    int LX;
    int LY;
    int RX;
};

RemoteData myData;


void setup() {
    Serial.begin(115200);
    delay(1000);

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