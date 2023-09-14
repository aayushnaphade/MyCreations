
#include <Adafruit_Fingerprint.h>
#include<HardwareSerial.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
const char* ssid = "Reading Room";
const char* password = "Password@4321";
const char* host = "script.google.com";
const String scriptUrl = "/macros/s/AKfycbxywP_ft0n0Bez9YvcoQRSg6qbtP8Zsv4zSneDJOspCY3UoIoy1XjUEGx4e290j7YUQ/exec";
//https://script.google.com/macros/s/AKfycbze_SvlHdZRCeB0tg_3WpIE9oCSJaj8seEzV_RyZjF7xtDo2vyLQh193FStn4PrW1MC/exec
//https://script.google.com/macros/s/AKfycbxywP_ft0n0Bez9YvcoQRSg6qbtP8Zsv4zSneDJOspCY3UoIoy1XjUEGx4e290j7YUQ/exec
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)



#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
//#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
String sendhttpreq(int fingerid){
    // Create an HTTPS client and disable certificate verification
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = "https://" + String(host) + scriptUrl + "?fingerid=" + String(fingerid);
  Serial.println(url);
  // Initialize the HTTPS connection
  // https.begin(client, host, 443, url);
  https.begin(url.c_str());
   https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
   // http.setFollowRedirects(HTTPC_FOLLOW_REDIRECTS_ALL);
    int httpCode = https.GET();
    if (httpCode > 0) {
      return https.getString();
    } else {
      return "Error: " + https.errorToString(httpCode);
    }

    https.end();
   
}
void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
    // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

void loop()                     // run over and over again
{
  getFingerprintID();
  delay(50);            //don't ned to run this at full speed.
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  String response = sendhttpreq(finger.fingerID);
  Serial.println(response);
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  String response = sendhttpreq(finger.fingerID);
  Serial.println(response);
  return finger.fingerID;
}
