#include <esp_now.h>
#include <WiFi.h>
#include "BluetoothSerial.h"
#include <Arduino.h>
#include <iostream>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <stdio.h>
#include <string.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif



// Universal MAC Address
uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0xF5, 0x45, 0x20};

String receivedMessage;
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

char a;
String receivedText;
String decrypted;
std::map<char, char> char_map;

void initialize_char_map() {
char_map['b'] = '0';
char_map['d'] = '1';
char_map['g'] = '2';
char_map['i'] = '3';
char_map['l'] = '4';
char_map['n'] = '5';
char_map['q'] = '6';
char_map['s'] = '7';
char_map['v'] = '8';
char_map['x'] = '9';
char_map['f'] = 'b';
char_map['k'] = 'd';
char_map['o'] = 'g';
char_map['p'] = 'i';
char_map['t'] = 'l';
char_map['u'] = 'n';
char_map['y'] = 'q';
char_map['z'] = 's';

}


String encrypt_string(const char *input) {
   String encrypted="";

    for (int i = 0; i < strlen(input); i++) {
        char ch = input[i];
        if (char_map[ch] != 0) {
            encrypted += char_map[ch];
        } else {
            encrypted += ch;
    }
    }
    return encrypted;
}

String decrypt_string(const char *input) {
    String decrypted = "";
    for (int i = 0; i < strlen(input); i++) {
        char ch = input[i];
        // Reverse the mapping by searching for the character in the char_map
        // and appending the original character if found
        bool found = false;
        for (const auto& entry : char_map) {
            if (entry.second == ch) {
                decrypted += entry.first;
                found = true;
                break;
            }
        }
        // If the character is not found in the char_map, append it as is
        if (!found) {
            decrypted += ch;
        }
    }
    return decrypted;
}

String readFile(fs::FS &fs, const char *path) {
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "";  // Return an empty string to indicate failure
    }

    String fileContent = "";
    while (file.available()) {
        char c = file.read();
        fileContent += c;

        // Print a newline character after each line
        if (c == '\n') {
            delay(10); // Optional delay for better readability
        }
    }

    file.close();
    return fileContent;
}


void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  //Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      //Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}



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
  Serial.println(status);
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
    String encryptedMessage = encrypt_string(receivedMessage.c_str());
    //delay(1000);
    Serial.print(encryptedMessage);
    appendFile(SD, "/hello.txt", encryptedMessage.c_str());
    
    // Clear the received message for the next one
    receivedMessage = "";
    
  }
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
  initialize_char_map();
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  //writeFile(SD, "/hello.txt", "Hello ");
  //appendFile(SD, "/hello.txt", "World!\n");
  //readFile(SD, "/hello.txt");
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
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
      if (myData.but_status == '~') {
    String a = readFile(SD, "/hello.txt");
    Serial.println(a);
    decrypted = decrypt_string(a.c_str());
    Serial.println(decrypted);
    myData.but_status = '\0';
  } else {

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
    }
    delay(20);
  }
  // Set values to send
} 