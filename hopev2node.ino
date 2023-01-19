#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
const char* ssid     = "home";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "123456789";    // The password of the Wi-Fi network
#define RST_PIN  D3     // Configurable, see typical pin layout above
#define SS_PIN   D4 
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key; 
const uint8_t fingerprint[20] = {0xA1, 0x89, 0xB2, 0x3F, 0x2B, 0x23, 0x53, 0x02, 0x95, 0x99, 0xAB, 0x68, 0x4D, 0x36, 0x7B, 0xD0, 0x87, 0xED, 0x32, 0x3D};
const String data1 = "https://script.google.com/macros/s/AKfycbxXV17v2jcHP19zm-5m2yPsH01XwC3CjNNVyK3xBzSRXzfAgtA7p2uMUdHRM8NPYImJrw/exec?cardno=";
String data2;
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
//        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
//        Serial.print(buffer[i], HEX);
    }
    
}

void setup() {
  Serial.begin(9600);         // Start the Serial communication to send messages to the computer
  delay(10);
//  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
//  Serial.print("Connecting to ");
//  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
//    Serial.print(++i); Serial.print(' ');
  }
Serial.println("6");
delay(500);
//  Serial.println('\n');
//  Serial.println("Connection established!");  
//  Serial.print("IP address:\t");
//  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
   for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
  
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("5");
  delay(1000);
}


void loop() { 
    if(WiFi.status() != WL_CONNECTED){
      Serial.println("7");
    } 
   if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
    Serial.println("1");
    byte sector         = 2;
    byte blockAddr      = 8;
    byte trailerBlock   = 11;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

//     Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
//        Serial.print(F("PCD_Authenticate() failed: "));
//        Serial.println(mfrc522.GetStatusCodeName(status));
          Serial.println("2");
          delay(100);
        return;
    }
    

//    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
//    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
//        Serial.print(F("MIFARE_Read() failed: "));
//        Serial.println(mfrc522.GetStatusCodeName(status));  
    }
  String data3 = data1 + String(buffer[0]) + String(buffer[1]) + String(buffer[2]) + String(buffer[3]);
//  Serial.println(data3);
    Serial.println("3");
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setFingerprint(fingerprint);
  HTTPClient https;
//  Serial.print(F("[HTTPS] begin...\n"));
    if (https.begin(*client, (String)data3))
    {  
      // HTTP
//      Serial.print(F("[HTTPS] GET...\n"));
      // start connection and send HTTP header
      int httpCode = https.GET();
    
      // httpCode will be negative on error
      if (httpCode > 0) 
      {
        // HTTP header has been send and Server response header has been handled
//        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
      }
      else 
      {
//        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
      delay(1000);
    } 
    else 
    {
//      Serial.printf("[HTTPS} Unable to connect\n");
    }
    Serial.println("4");
   
     mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}
