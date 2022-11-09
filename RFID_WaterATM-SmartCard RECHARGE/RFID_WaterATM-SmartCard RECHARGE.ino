/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 *
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 *
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 *
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 *
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 *
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
LiquidCrystal_I2C lcd(0x27, 16, 2);
int buzzer = 8;
int s1 = 2;
int s2 = 3;
int s3 = 4; //for mode selecter
int s4 = 5;
int s5 = 6;
int balpin = 7;

bool a = false;
bool b = false;

int selector = 0;

byte onece;
byte tens;
byte hundred;

byte bone;
byte bten;
byte bhundred;


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

/**
 * Initialize.
 */
void setup() {
    Serial.begin(9600);// Initialize serial communications with the PC
    lcd.init();
    // Turn on the blacklight and print a message.
    lcd.backlight();
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
    pinMode(buzzer , OUTPUT);
    pinMode(s1 , INPUT_PULLUP);
    pinMode(s2 , INPUT_PULLUP);
    pinMode(s3 , INPUT_PULLUP);
    pinMode(s4 , INPUT_PULLUP);
    pinMode(s5 , INPUT_PULLUP);
    pinMode(balpin, INPUT_PULLUP);
    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Recharge Machine Remodel by Ayush."));
    Serial.print(F("Accesing Card With KEY:"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    lcd.setCursor(0,0);
    lcd.print("Mode->");

    Serial.println(F(""));
    if (digitalRead(s1) == LOW){
      selector = 1;
      lcd.setCursor(7,0);
      lcd.print("Rs.100");
    }
   
    else if(digitalRead(s3) == LOW){
      selector = 4;
      lcd.setCursor(7,0);
      lcd.print("NewCards");
    }
    else if(digitalRead(s4) == LOW){
      selector = 2;
      lcd.setCursor(6,0);
      lcd.print("OtherVendor");
    }
    else if(digitalRead(s2) == LOW){
      a = true;
      selector = 5;
      lcd.setCursor(7,0);
      lcd.print("CardReset");
    }
    else {
      selector = 0;
      lcd.setCursor(7,0);
      lcd.print("Rs.50");
    }
    
    Serial.print("Selector:");
    Serial.println(selector);
    lcd.setCursor(0,1);
    lcd.print("Balence ->");
}

/**
 * Main loop.
 */
void loop() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    if(digitalRead(balpin) == 0){
      b = true;
      }
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 2;
    byte blockAddr      = 8;
    byte trailerBlock   = 11;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    byte dataBlock[]    = {
        0x00, 0x00, 0x00, 0x00, //  1,  2,   3,  4,
        0x00, 0x00, 0x00, 0x00, //  5,  6,   7,  8,
        0x00, 0x00, 0x00, 0x00, //  9, 10, 255, 11,
        0x00, 0x00, 0x00, 0x00 };
   

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        delay(100);
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        
        return;
    }

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Read data from the block
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        delay(100);
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        
    }
    bone = buffer[15];
    bten = buffer[14];
    bhundred = buffer[13];
    if(b == false){
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
   while(a == false){     // for dial 5 =  for erasing the balance
    if (selector == 0){  //for dial 0 = 50 recharge
    onece = buffer[15];
    tens  = buffer[14];
    hundred = buffer[13];
    dataBlock[15] = onece;
    dataBlock[13] = hundred;
    tens = tens + 0x05;
    if(tens > 10 || tens == 10){
      dataBlock[14] = tens - 10;
      dataBlock[13] = hundred + 0x01;
    }
    else dataBlock[14] = tens;
    }
    if (selector == 1){  //for dail 1 = 100 rupess
    onece = buffer[15];
    tens  = buffer[14];
    hundred = buffer[13];
    dataBlock[15] = onece;
    dataBlock[14] = tens;
    hundred = hundred + 0x01;
    if(tens > 0x09|| tens == 0x0A){  //if the rechage is above 999 then it reset 
      dataBlock[13] = 0x00;        
      dataBlock[14] = 0x00;
      dataBlock[15] = 0x00;
    }
    else dataBlock[13] = hundred;
    }
    if(selector == 4){             //for making new card for rupee 100 with bal 65
      dataBlock[13] = 0x00;
      dataBlock[14] = 0x06;
      dataBlock[15] = 0x05;
    }
    if(selector == 2){

      byte data[] = { 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x00, 0x00, 0x00,
                 0x00, 0x06, 0x08, 0x07};
                
      // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        delay(100);
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        
        return;
    }

    // Write data to the block
    Serial.print(F("Writing data into identity block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(9, data, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        delay(100);
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        
    }
    Serial.println();

    }
    break;
   }


    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        delay(100);
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        
        return;
    }

    // Write data to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        delay(100);
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        
    }
    Serial.println();

    // Read data from the block (again, should now be what we have written)
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        delay(100);
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();

    // Check that data in block is what we have written
    // by counting the number of bytes that are equal
    Serial.println(F("Checking result..."));
    byte count = 0;
    for (byte i = 0; i < 16; i++) {
        // Compare buffer (= what we've read) with dataBlock (= what we've written)
        if (buffer[i] == dataBlock[i])
            count++;
    }
    Serial.print(F("Number of bytes that match = ")); Serial.println(count);
    if (count == 16) {
        Serial.println(F("Success :-)"));
    } else {
        Serial.println(F("Failure, no match :-("));
        Serial.println(F("  perhaps the write didn't work properly..."));
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        delay(100);
        digitalWrite(buzzer , HIGH);
        delay(100);
        digitalWrite(buzzer , LOW);
        
    }
    Serial.println();
    }
    // Dump the sector data
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

   
    

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    
    if(b == false){
       lcd.setCursor(11,1);
       lcd.print(dataBlock[13]);
       lcd.setCursor(12,1);
       lcd.print(dataBlock[14]);
       lcd.setCursor(13,1);
       lcd.print(dataBlock[15]);
    }
    else
    {
       lcd.setCursor(11,1);
       lcd.print(bhundred);
       lcd.setCursor(12,1);
       lcd.print(bten);
       lcd.setCursor(13,1);
       lcd.print(bone);
       b = false;
    }
       onece = 0x00;
       hundred = 0x00;
       tens = 0x00;
       digitalWrite(buzzer , HIGH);
       delay(300);
       digitalWrite(buzzer , LOW);
       delay(1500);
       lcd.setCursor(11,1);
       lcd.print(".....");  

       for(int p = 0; p > 16 ; p ++){
      dataBlock[p] = 0x00;
    }
     for(int q = 0; q < size ; q++){
      buffer[q] = 0x00;
    }
    
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    
}
