#include <esp_now.h>
#include <WiFi.h>

uint8_t A_address[] = {0xA0, 0xB7, 0x65, 0x58, 0x7A, 0xFC};  // AのMACアドレス
int Send1;
int Send2;
int ADC = 0;  // BのADCデータを格納

typedef struct struct_message {
  int data[5];
} struct_message;

struct_message receivedData;

// Aからのデータ受信コールバックconst esp_now_recv_info *info
void onReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  // データ表示
  Serial.print("BがAから受信したデータ: ");
  Serial.println(receivedData.data[1]);
  Serial.println(receivedData.data[2]);
  Serial.println(receivedData.data[3]);

  // ADC値を更新
  receivedData.data[3] = 300; // GPIO34ピンからADC読み取り

  // 更新したデータをAに送信
  esp_now_send(A_address, (uint8_t *) &receivedData, sizeof(receivedData));
  Serial.println("Bが更新されたADCデータをAに返送しました");
}

// 送信完了のコールバック
void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("送信ステータス: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "成功" : "失敗");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOWの初期化に失敗しました");
    return;
  }

  esp_now_register_recv_cb(onReceive);
  esp_now_register_send_cb(onSent);

  // Aのピア情報を追加
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, A_address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Aのピア追加に失敗しました");
    return;
  }
}

void loop() {
  delay(500);
}
