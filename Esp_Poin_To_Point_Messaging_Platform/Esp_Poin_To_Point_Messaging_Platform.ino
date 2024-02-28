#include <esp_now.h>
#include <WiFi.h>
#include "BluetoothSerial.h"
#include <Arduino.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif



// Universal MAC Address
uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0xF5, 0x45, 0x20};

String receivedMessage = "";
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char but_status;
} struct_message;
struct_message incomingReadings;
struct_message myData;
esp_now_peer_info_t peerInfo;

BluetoothSerial SerialBT;
boolean confirmRequestPending = false;


void BTConfirmRequestCallback(uint32_t numVal)
{
  confirmRequestPending = true;
  Serial.println(numVal);
}

void BTAuthCompleteCallback(boolean success)
{
  confirmRequestPending = false;
  if (success)
  {
    Serial.println("Pairing success!!");
  }
  else
  {
    Serial.println("Pairing failed, rejected by user!!");
  }
}


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  
   for (int i = 0; i < len; i++) {
    receivedMessage += (char)incomingData[i]; // Append each character to the received message
  }
  //Serial.print(receivedMessage);
  // Check for a termination condition, such as a newline character
  if (receivedMessage.endsWith(";")) {
    // Process the received message here
    SerialBT.print(receivedMessage);
    Serial.print(receivedMessage);

    // Clear the received message for the next one
    receivedMessage = "";
    
  }

 /* memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  //String receivedString = String(incomingReadings.but_status);
  //size_t length = incomingReadings.but_status.length();
  SerialBT.print(incomingReadings.but_status);
  //delay(100);
  Serial.println(incomingReadings.but_status);
  //Serial.write(incomingReadings.but_status);
  //SerialBT.write(incomingReadings.but_status);
*/
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);


  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Send Callback Function
  esp_now_register_send_cb(OnDataSent);

  // Receive Callback Function
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = true;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  SerialBT.enableSSP();
  SerialBT.onConfirmRequest(BTConfirmRequestCallback);
  SerialBT.onAuthComplete(BTAuthCompleteCallback);
  SerialBT.begin("ESP32test2"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop()
{
    if (confirmRequestPending)
  {
    if (Serial.available())
    {
      int dat = Serial.read();
      if (dat == 'Y' || dat == 'y')
      {
        SerialBT.confirmReply(true);
      }
      else
      {
        SerialBT.confirmReply(false);
      }
    }
  }
  else
  {
    if (Serial.available())
    {
      SerialBT.write(Serial.read());
    }
    if (SerialBT.available())
    {
      //String mass;
      myData.but_status = SerialBT.read();
      //myData.but_status = mass;
      Serial.println(myData.but_status);
      //Serial.write(myData.but_status);
      //SerialBT.write(myData.but_status);
      
      esp_err_t result;
      delay(100);
      result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        if (result == ESP_OK) 
        {
          Serial.println("Sent with success");
        }
        else
        {
          Serial.println("Error sending the data");
        }

    }
    delay(20);
  }
  // Set values to send
} 