

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <Adafruit_Fingerprint.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WebServer.h>
#elif defined(TARGET_RP2040)
  #include <WiFi.h>
  #include <WebServer.h>
#endif

#include <ElegantOTA.h>

const char* ssid = "HB-6 Super";
const char* password = "hb6@1234";
#define STATIC_IP true
#ifdef STATIC_IP
// Configure your statis IP details.
IPAddress local_IP(192, 168, 0, 107);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 4, 4);    //optional
IPAddress secondaryDNS(8, 8, 8, 8);  //optional
#endif
int a = 27;
int red = 17;
int blue = 5;
int green = 4;
unsigned long previousMillis = 0;
const long helloDelay = 2000; // Delay for "Hello" in milliseconds
const long worldDelay = 3000; // Delay for "World" in milliseconds
boolean helloPrinted = false;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial);
#define DEBUG true
const char* websockets_server_host = "192.168.0.173";  //Enter server adress
const uint16_t websockets_server_port = 1880;          // Enter server port
bool re = false;
bool qiqi = false;

#if defined(ESP8266)
  ESP8266WebServer server(80);
#elif defined(ESP32)
  WebServer server(80);
#elif defined(TARGET_RP2040)
  WebServer server(80);
#endif

using namespace websockets;

WebsocketsClient client;

// JSON Desrializer
DynamicJsonDocument doc(1024);

char response[200];
bool connectionStatus = false;


unsigned long ota_progress_millis = 0;
void idel() {
  static unsigned long previousMillis = 0;
  static int step = 0;
  unsigned long currentMillis = millis();

<<<<<<< HEAD

=======

>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a
  switch (step) {
    case 0:
      if (currentMillis - previousMillis >= helloDelay) {
        digitalWrite(green, LOW);
        digitalWrite(red, LOW);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("....Welcome.....");
        lcd.setCursor(0, 1);
        lcd.print("................");
        previousMillis = currentMillis;
        step = 1;
      }
      break;

    case 1:
      if (currentMillis - previousMillis >= worldDelay) {
        lcd.clear();
        digitalWrite(green, LOW);
        digitalWrite(red, LOW);
        lcd.setCursor(0, 0);
        lcd.print("Tip:- keep your");
        lcd.setCursor(0, 1);
        lcd.print("fingers clean...");
        previousMillis = currentMillis;
        step = 0; // Reset step to 0 for the next cycle
      }
      break;
  }
}
void tryagain(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Try Again........");
  digitalWrite(a ,HIGH);
  delay(70);
  digitalWrite(a ,LOW);
  delay(30);
  digitalWrite(a ,HIGH);
  delay(60);
  digitalWrite(a ,LOW);
  delay(50);
  digitalWrite(a ,HIGH);
  delay(87);
  digitalWrite(a ,LOW);
  delay(2000);
  lcd.clear();
  //idel();
  return;
}
void temp(String d){
  lcd.setCursor(0,1);
  lcd.print(d);
  delay(2800);
  lcd.clear();
  lcd.setCursor(0,0);
 //idel();
}

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

void setup(void) {
  #ifdef STATIC_IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    //Serial.println("STA Failed to configure");
  }
#endif

  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print("-");
<<<<<<< HEAD

  }

  lcd.init();
  // Make sure backlight is on
=======

  }

  lcd.init();
  // Make sure backlight is on
>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a
  lcd.backlight();
  pinMode(a , OUTPUT);
  pinMode(green , OUTPUT);
  pinMode(blue , OUTPUT);
  pinMode(red , OUTPUT);
<<<<<<< HEAD

=======

>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Starting Client!");
  lcd.setCursor(0,1);
//  lcd.print("..........V.0.9");
  lcd.print("Initalizing Node");


  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    //found sensor
  } else {
<<<<<<< HEAD

=======

>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a
    while (1) { delay(1); }
  }
  finger.getParameters();
  finger.getTemplateCount();

  delay(1380);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("H.O.P.E by SPARK");
  lcd.setCursor(0,1);
  lcd.print("..........V.2.1");
//  lcd.print("Aayush....VNIT");
  digitalWrite(a ,HIGH);
  delay(280);
  digitalWrite(a ,LOW);
  delay(3800);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("_HB8~HB6 Hostel_");
  lcd.setCursor(0,1);
  lcd.print("....V.N.I.T.....");
  delay(2800);
  lcd.clear();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
<<<<<<< HEAD

=======

>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a
  }
  //Serial.println(WiFi.localIP());
  server.on("/", []() {
    String imageUrl = "https://i.imgur.com/udr39Ku.jpg"; // Replace with your direct image URL
    String htmlCode = "<html><body style='text-align:center;'><h1>FingerPrint Attendance Management System<br>by S.P.A.R.K Workshop and Kaustubh Sir</h1><img src='" + imageUrl + "'></body></html>";
    server.send(200,"text/html", htmlCode);
  });
  ElegantOTA.setAuth("spark", "Ayushnaphade1@");
  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
<<<<<<< HEAD

  bool connected = client.connect(websockets_server_host, websockets_server_port, "/test2/hb6");
  if (connected) {

    connectionStatus = true;
    digitalWrite(blue, HIGH);
  } else {

  }

  client.onMessage(messageCallback);
  client.onEvent(eventCallback);


=======

  bool connected = client.connect(websockets_server_host, websockets_server_port, "/test2/hb6");
  if (connected) {

    connectionStatus = true;
    digitalWrite(blue, HIGH);
  } else {

  }

  client.onMessage(messageCallback);
  client.onEvent(eventCallback);


>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a
  client.ping();
}

void loop(void) {
  server.handleClient();
    idel();
  if (client.available()) {
    client.poll();
  }
  if(qiqi == false){
  getFingerprintID();
  }
  if(re == true){
    tryagain();
    re = false;
  }
<<<<<<< HEAD


=======


>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a

  if (!connectionStatus) {
    //Serial.println("No Connection");
    digitalWrite(blue, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Buffer Tunnel");
    lcd.setCursor(0,1);
    lcd.print("Disconected...");
    bool reconnectStatus = client.connect(websockets_server_host, websockets_server_port, "/test2/hb6");
    if (reconnectStatus) {
    //  Serial.println("");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Node Reconnected");
      connectionStatus = true;
      digitalWrite(blue, HIGH);
      qiqi = false;
    } else {
      // 1 Min Wait
      delay(6000);
      //node error...
    }
  }
  ElegantOTA.loop();
}

void messageCallback(WebsocketsMessage message) {
  deserializeJson(doc, message.data());
  if (doc["payload"] == "IN") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  .....IN....  ");
    lcd.setCursor(0, 1);
    lcd.print("................");
    digitalWrite(green, HIGH);
    digitalWrite(a, HIGH);
    delay(120);
    digitalWrite(a, LOW);
<<<<<<< HEAD

=======

>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a
    delay(1500);
    qiqi = false;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   ....OUT.... ");
    lcd.setCursor(0, 1);
    lcd.print("................");
    digitalWrite(red, HIGH);
    digitalWrite(a, HIGH);
    delay(120);
    digitalWrite(a, LOW);
    delay(95);
    digitalWrite(a, HIGH);
    delay(120);
    digitalWrite(a, LOW);
<<<<<<< HEAD

=======

>>>>>>> bf64dd1fb9abe0f5afad6b4ed2132e623e2ff93a
    delay(1500);
    qiqi = false;
  }
}

void eventCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    //Serial.println("Connnection Opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    //Serial.println("Connnection Closed");
    connectionStatus = false;
  } else if (event == WebsocketsEvent::GotPing) {
    //Serial.println("Pinged👋, responding with PONG!");
    client.pong();
  } else if (event == WebsocketsEvent::GotPong) {
    //Serial.println("Got a Pong!");
  }
}

//finger
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:

      break;
    case FINGERPRINT_NOFINGER:

      return p;
    case FINGERPRINT_PACKETRECIEVEERR:

      return p;
    case FINGERPRINT_IMAGEFAIL:

      return p;
    default:

      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:

      break;
    case FINGERPRINT_IMAGEMESS:

      re = true;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      re = true;

      return p;
    case FINGERPRINT_FEATUREFAIL:
       re = true;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
        re = true;

      return p;
    default:
      re = true;
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {

     re = true;
     return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    re = true;
    return p;
  } else {

    re = true;
    return p;
  }

  // found a match!
//  Serial.print("Found ID #"); Serial.print(finger.fingerID);
//  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  lcd.clear();
  String fingerid = String(finger.fingerID);
  sprintf(response, "%s",fingerid);
  qiqi = true;
  client.send(response);
  delay(50);
//  Serial.println(response);
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    re = true;
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    return -1;
    re = true;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    return -1;
    re = true;
  }

  String fingerid = String(finger.fingerID);
  sprintf(response, "%s",fingerid);
  client.send(response);
  qiqi = true;
  temp(response);

  return finger.fingerID;
}
