#include <esp_now.h>
#include <WiFi.h>
int Send1;
int Send2;
int ADC;
// データ構造をBと一致させる
typedef struct struct_message {
  int data[5]; // Aからのデータ
} struct_message;

struct_message receivedData;
 
// 新しいシグネチャに合わせてコールバック関数を修正
void onReceive(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("CがAからのデータを受信: ");
  Serial.println(receivedData.data[1]);
  Serial.println(receivedData.data[2]);
  Serial.println(receivedData.data[3]);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() == ESP_OK) {
    esp_now_register_recv_cb(onReceive);
  } else {
    Serial.println("ESP-NOWの初期化に失敗しました");
  }
}

void loop() {
  // 特に何もしない、受信待機のみ
  // 受信データの設定
  if (receivedData.data[0] == 99 && receivedData.data[4] == 88) {      // ヘッダー
    Send1 = receivedData.data[1];    // 送信するデータ
    Send2 = receivedData.data[2];    // 送信するデータ
    ADC = receivedData.data[3];      // ADCのデータ
    Serial.print("Send1  ");Serial.println(Send1);
    Serial.print("Send2  ");Serial.println(Send2);
    Serial.print("ADC  ");Serial.println(ADC);
}
delay(500);
}
