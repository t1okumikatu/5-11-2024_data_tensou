#include <esp_now.h>
#include <WiFi.h>

uint8_t B_address[] = {0x64, 0xE8, 0x33, 0x50, 0x9D, 0x24};  // BのMACアドレス
uint8_t C_address[] = {0x84, 0xFC, 0xE6, 0x84, 0x30, 0x58};  // CのMACアドレス

int Send1;
int Send2;
int ADC = 0;
int BADC = 0;

typedef struct struct_message {
  int data[5];
} struct_message;

struct_message sendData;

// 送信完了のコールバック
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("送信ステータス: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "成功" : "失敗");
}

// Bからのデータ受信コールバック
void onReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  struct_message receivedData;
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.print("AがBから受信したデータ（更新後のADC）: ");
  Serial.println(receivedData.data[3]);
  BADC = receivedData.data[3];
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOWの初期化に失敗しました");
    return;
  }

  esp_now_register_send_cb(onSent);
  esp_now_register_recv_cb(onReceive);

  // Bのピア情報を追加
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, B_address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Bのピア追加に失敗しました");
    return;
  }

  // Cのピア
  memcpy(peerInfo.peer_addr, C_address, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Cのピア追加に失敗しました");
    return;
  }
}

void loop() {
  Send1 = 100;
  Send2 = 200;
  ADC = BADC; // Bから受信した最新のADCデータを使用

  sendData.data[0] = 99;    // ヘッダー
  sendData.data[1] = Send1;
  sendData.data[2] = Send2;
  sendData.data[3] = ADC;
  sendData.data[4] = 88;    // フッター

  if (sendData.data[0] == 99 && sendData.data[4] == 88) {
    esp_now_send(B_address, (uint8_t *) &sendData, sizeof(sendData)); // Bへ送信
    esp_now_send(C_address, (uint8_t *) &sendData, sizeof(sendData)); // Cへ送信
    Serial.println("AからBとCにデータを送信しました");
  }
  Serial.print("Send1  ");
  Serial.println(Send1);
  Serial.print("Send2  ");
  Serial.println(Send2);
  Serial.print("ADC  ");
  Serial.println(ADC);
  delay(500);
}
