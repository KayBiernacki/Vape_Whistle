#include "BluetoothA2DPSink.h"
#include "esp_sleep.h"
#include "esp_gap_bt_api.h"

#define BUTTON_PIN GPIO_NUM_4

BluetoothA2DPSink a2dp_sink;

// Broadcast simply makes BT device visible/
void startBroadcasting() {
  esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
  a2dp_sink.reconnect(); 
}

// This disconnects Bluetooth
void stopBroadcasting() {
  if (a2dp_sink.is_connected()) {
    a2dp_sink.disconnect();
  }
  esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
}

void enterLightSleep() {
  delay(100);

  gpio_wakeup_enable(BUTTON_PIN, GPIO_INTR_HIGH_LEVEL);
  esp_sleep_enable_gpio_wakeup();

  esp_light_sleep_start();

  Serial.println("Woken up from Light Sleep by button!");
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN); 

  a2dp_sink.set_auto_reconnect(true, 1000);
  a2dp_sink.start("Panic1");

  stopBroadcasting();
}

void loop() {

  enterLightSleep();

  unsigned long pressStart = millis();
  while (digitalRead(BUTTON_PIN) == HIGH) {
    delay(50);
    if (millis() - pressStart > 10000) break; 
  }
  
  unsigned long holdDuration = millis() - pressStart;

  if (holdDuration >= 100) {
    startBroadcasting();

    unsigned long broadcastStartTime = millis();
    while (millis() - broadcastStartTime < 10000) {
      if (a2dp_sink.is_connected()) {
        delay(2000); 
        break;
      }
      delay(100);
    }

    stopBroadcasting();
  } else {
 
  }

}