/*
 * Displays text sent over the serial port (e.g. from the Serial Monitor) on
 * an attached LCD.
 * YWROBOT
 *Compatible with the Arduino IDE 1.0
 *Library version:1.1
 */
//#include <Serial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
int buzzer  = 2;
int blue = 7;
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  pinMode(buzzer, OUTPUT);
  pinMode(blue , OUTPUT);
  digitalWrite(buzzer, HIGH);
  delay(250);
  digitalWrite(buzzer, LOW);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  Serial.begin(9600);
  lcd.setCursor(1,0);
  lcd.print(".....SPARK......");
  lcd.setCursor(0,1);
  lcd.print("HB6 & HB7 VNIT");
  delay(2500);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Loading....");
  
}

void loop()
{
  int a;
  // when characters arrive over the serial port...
  if (Serial.available()) {
     if (Serial.available() > 0) {
    a = Serial.parseInt();
    Serial.println(a);
  }
    
    // wait a bit for the entire message to arrive
    if(a == 1){
      lcd.clear();
      lcd.print("Processing...");
    }
    else if(a == 3){
      lcd.clear();
      lcd.print("You can Go");
      lcd.setCursor(0,1);
      lcd.print("Wait........");
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
      delay(250);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(buzzer, LOW);
    }
    else if(a == 4){
      lcd.clear();
      lcd.print("Done!!");
    }
    else if(a == 2){
      lcd.clear();
      lcd.print("Too Fast");
      lcd.setCursor(0,1);
      lcd.print("Try Again");
      
    }
    else if(a == 5){
      lcd.clear();
      lcd.print("Place Your Card");
    }
    else if(a == 6){
      digitalWrite(blue ,HIGH);
    }
    else if(a == 7){
      digitalWrite(blue, LOW);
    }
  }
}
