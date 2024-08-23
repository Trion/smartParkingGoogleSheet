


#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP_Mail_Client.h>
#include <SPI.h>
#include <MFRC522.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>


LiquidCrystal_I2C lcd(0x27,16,2);

#define WIFI_SSID "BabyAncestoriNteli" // မိမိချိတ် မည့် ၀◌ိုင်ဖိုင် နာမည် နှင့် ကုတ် ကို ထည့်သွင်းအသုံးပြုရန် ။
#define WIFI_PASSWORD "bb2020$!@@"

// အ၀◌င် ဆင်ဆာ ပင်များ
#define gate1_trig 13
#define gate1_echo 12
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

#define gate2_trig 14
#define gate2_echo 27

long gate1_duration;
long gate2_duration;

float gate1_distacecm; 
float gate2_distancecm;

bool scan_rfid = false;
/** The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com */
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "espdev909@gmail.com"
#define AUTHOR_PASSWORD "gsxr vlyh xnwf xyrw"

/* Recipient's email ကိုပို့ချင်တယ့် မေလ် ကိုထည့်ရန် */
#define RECIPIENT_EMAIL "nyeinchanko0ko@gmail.com" //change receive mail here

#define CLIENT_EMAIL "smartparkinglog@smartparkingdatalogging.iam.gserviceaccount.com"// for google sheet log

const char PRIVATE_KEY[] PROGMEM  = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCWeJfAfXBz59F+\n1AcOaQASK3FLaOsg6s4RbZ7kvoUZpA/Jhqi5P/opp5YlIit4IwhP/7ak8hHVGIbl\nVl/Y06ll3NRNpVLA/MIPm3iej0qSNz/3dEMRx1Ec7TqVzzPqdunNcmOoM33wyyhi\nj1afBVx+er4Jqq63Zl39tWMCKxxZambPN5pDsZH3tDX+jivKl7SpdYb4hmKtJO7p\nK/lg2mj/grx4L1TTIz9eHXSSGK0YSZOipHyOvPpWjjtCG7jh3bivFLqFEf98Ny5P\n6blCUnLEIn9JeyX/d9uWeBFTVrPr8wuGLyiZznC45nKcewW2Q6eeqLDbu12lVjpE\ntJLNZRNfAgMBAAECggEABCv5wRt5N61wsHx+aZbdqqNLs9SH2FTKO893PU5O8GQN\nEABMwz4T2H3N+t+UpvMJsvYbAT244tiYPNmFNMTBiJJzzGdOAQ3A75/W20aLwbwI\naPrvNRn6lLu7CRZCkTiUyN5oBuzOjBOF3h2tQCFs7BDJHgl+aJVM+9DAmvnJAm2W\n45Ey9JVZp4sqO9E7hxTnYnfSLIXkKJVgC30CfbPtrNHWp8aOFg+Whv6WshnRxlFr\nyJxyXqE47f9czwac5K+hhSDf74yhK/56R9Vu/9IBiUhBAFueq82+3k/y/dNP0VGD\n+5ucepranhGEIrsa+jLRjgXErt1aqNFdLbWti/4zEQKBgQDH9NMZSL3t0CLSBBki\nc8xHmSpCnzBzTNZDmfcQjzsIlRgstHptuQ1VXIaupu2gsqPKnoiJeUhmV3LapOvj\nvmlWuo6HSUzECDDSkWh/9X5YraNctmRWFTKZ+lq3lCJHh5HHT+4pCb7a7kf64bvo\ndmz8fLX50r8RPy40B5YrTm/r9wKBgQDApR+Iwo1SB5xpVbBxsggVqJGfUQOO8IFN\nNor/Qtcdzzjmm6nuw5/DmgRuL9nFglyl7fTs37E5Ozn6abO37ovh6yt0+Fg4sDwj\n6D+7lsu9MyCOjV2tMjUZlqVEpRjb47Lw7XhbQLEoDEFzU7iIkOFFJkUtk54b6HpO\nOVW9EECp2QKBgDwe763lgj3T2/RbiRcjzCMm+V7osE+qpa3TyEZxWmWdNpSSe1Es\nV8b3uAMha8dqjvboLdNv5QXoez8kYUyrvs51707cCjnS4KMfV1VKAgA1zS2eJZMA\ncqN7c/VIk/n/2CQec1RhesIm7PuRDce6e9B6ythftlVAMusv8MX6oopHAoGBALNb\nyV1SDbNW5LhpkGp6n+5SUKptCgwWPbx+0LDSMWhOm6KRQYjMV74Mcp2ILZAnTE0s\nnEHCDsXDL7rGmQ3hqhkAFCjGY5T7+DJSNKxZvHz90g0G7zWmriuRx3N+5wMrH1oT\nLmB/fbFUn8dZJlD32tS/yd6iNq2zJ9QWq43oSNwZAoGBAIPM5spfeOXf3frIg6Vw\n81WSJJXPFgchuBLsJHe0PnXucmmTYuTYcOwKfD6RLdTuYgwFgOrGLlEK3yIm2Ldo\nd68XQztqBmxeAR08qNaqea9vWWe2sGjr5jtq8y+AJKjvz7TtDKXTe8S8w0cZwEkB\nSJn3Fl25tvUlM6e/jo8E7GgC\n-----END PRIVATE KEY-----\n";
const char SPREADSHEET_ID []= "1vchWsIglYT-P-uNIJoEhyFMXR_jKanujhQnq9JKtC-M";
#define PROJECT_ID  "smartparkingdatalogging"
/* Declare the global used SMTPSession object for SMTP transport */
SMTPSession smtp;
SMTP_Message message;


/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);
void tokenStatusCallback(TokenInfo info);
SMTP_Status status;

#define SS_PIN 5
#define RST_PIN 15
#define OUT_SDA 4

int car_count  = 4;
 
MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 
byte nuidPICC[4];

int time_zone = 6.5 * 3600;                                         //GMT +6:30
int dst = 0;       



void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}


void smtpCallback(SMTP_Status status){
  Serial.println(status.info());
  if (status.success()){
    lcd.setCursor(0,1);
    lcd.print("sentSuccess");
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {

      SMTP_Result result = smtp.sendingResult.getItem(i);
      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
    smtp.sendingResult.clear();
    lcd.clear();
  }
}

void tokenStatusCallback(TokenInfo info){
    if (info.status == token_status_error){
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}


void sendMail(String sender_name_,String subject_,String mail_) {
  Session_Config config;
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 6.5;
  config.time.day_light_offset = 0;
  if (!smtp.connect(&config)){
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn()){
    Serial.println("\nNot yet logged in.");
  }
  else{
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }
  message.sender.name = sender_name_;
  message.sender.email = AUTHOR_EMAIL;
  message.subject = subject_;
  message.addRecipient(F("Sara"), RECIPIENT_EMAIL);
    
  String textMsg = mail_;
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;
  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
 smtp.sendingResult.clear();
}

float check_gate1_sensor(){
  digitalWrite(gate1_trig,LOW);
  delayMicroseconds(2);
  digitalWrite(gate1_trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(gate1_trig,LOW);
  gate1_duration = pulseIn(gate1_echo,HIGH);
  gate1_distacecm = gate1_duration * SOUND_SPEED/2;
  Serial.print("GATE 1 SSENSOR : ");
  Serial.println(gate1_distacecm);
  return gate1_distacecm;
}

float check_gate2_sensor() {
  digitalWrite(gate2_trig,LOW);
  delayMicroseconds(2);
  digitalWrite(gate2_trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(gate2_trig,LOW);
  gate2_duration = pulseIn(gate2_echo,HIGH);
  gate2_distancecm = gate2_duration * SOUND_SPEED/2;
  Serial.print("GATE 2 SENSOR :");
  Serial.println(gate2_distancecm);
  return gate2_distancecm;
}

void gate1_scan_rfid() {
   if(scan_rfid) {
  digitalWrite(OUT_SDA,HIGH);
  Serial.println("READ ENTER GATE RFID");
   if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;
    //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  Serial.println(content.substring(1));
  time_t now = time(nullptr);
  String time_now = ctime(&now);
  Serial.println(time_now.c_str());
  if(content.substring(1) == "D2 C5 18 2E" ){
      FirebaseJson response;
      FirebaseJson valueRange;
       valueRange.add("majorDimension", "COLUMNS");
        valueRange.set("values/[0]/[0]", "USER2");
        valueRange.set("values/[1]/[0]", "D2 C5 18 2E");
        valueRange.set("values/[2]/[0]", "espdev909@gmail.com");
        valueRange.set("values/[3]/[0]", time_now.c_str());
       bool success = GSheet.values.append(&response /* returned response */, SPREADSHEET_ID /* spreadsheet Id to append */, "Sheet1!A1" /* range to append */, &valueRange /* data range to append */);
        if (success){
            response.toString(Serial, true);
            valueRange.clear();
        }
        else{
            Serial.println(GSheet.errorReason());
        }
        Serial.println();
        Serial.println(ESP.getFreeHeap());

    sendMail("USER2","USER2STATUS","CARDID: D2 C5 18 2E");
    car_count = car_count - 1;
    }
  else if(content.substring(1) == "54 41 CC 24"){
    sendMail("USER1","USER1STATUS","CARDID: 54 41 CC 24");
    car_count = car_count -1;
    }
  else if(content.substring(1)== "03 E7 53 FE"){
    sendMail("USER3","USER3STATUS","CARDID: 03E753FE");
     car_count = car_count -1;
    }
  else if(content.substring(1)== "54 88 29 24"){
    sendMail("USER4","USER4STATUS","CARDID: 54 88 29 24");
     car_count = car_count -1;
    }
  else Serial.println("NO REGISTERED CARD ERROR");

 } 
  else Serial.println(F("Card read previously."));
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  scan_rfid = false;
 }
}


void gate2_scan_rfid(){
   if(scan_rfid) {
  digitalWrite(OUT_SDA,HIGH);
  Serial.println("READ ENTER GATE RFID");
   if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if ( ! rfid.PICC_ReadCardSerial())
    return;
    //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  Serial.println(content.substring(1));
  lcd.clear();
  if (car_count == 4){
    lcd.setCursor(0,0);
    lcd.print("SCANNED ERROR ");
    delay(1000);
    lcd.clear();
  }
  else {
  if(content.substring(1) == "D2 C5 18 2E" ){
     car_count = car_count +1;
    sendMail("USER2","USER2STATUS","CARDID: D2 C5 18 2E");}
  else if(content.substring(1) == "54 41 CC 24"){
     car_count = car_count +1;
    sendMail("USER1","USER1STATUS","CARDID: 54 41 CC 24");}
  else if(content.substring(1)== "03 E7 53 FE"){
     car_count = car_count +1;
    sendMail("USER3","USER3STATUS","CARDID: 03E753FE");}
  else if(content.substring(1)== "54 88 29 24"){
     car_count = car_count +1;
    sendMail("USER4","USER4STATUS","CARDID: 54 88 29 24");}
  else Serial.println("NO REGISTERED CARD ERROR");
    }

 } 
  else Serial.println(F("Card read previously."));
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  scan_rfid = false;
 }
}



// မိန်း ဖန်ရှင် ဒိထဲမှာ pinout ‌ြေက ငြာတာတွေလုပ် ရမယ်။
void setup() { 
  Serial.begin(9600);
  pinMode(gate1_trig,OUTPUT);
  pinMode(gate1_echo,INPUT);
  pinMode(gate2_trig,OUTPUT);
  pinMode(gate2_echo,INPUT);
  pinMode(OUT_SDA,OUTPUT);

  lcd.init();
  lcd.backlight();

  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    lcd.setCursor(0,0);
    lcd.print("WiFiCONNECTING");
    Serial.print(".");
    delay(300);
    lcd.clear();
  }
  Serial.println();
  lcd.setCursor(0,0);
  lcd.print("WiFiCONNECTED");
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(time_zone,dst,"pool.ntp.org","time.nist.gov");
  Serial.println("Waiting For Internet Time");
  while(!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Time Response..... OK");


  // start rfid communication
  digitalWrite(4,HIGH);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
// end rfid communication

  MailClient.networkReconnect(true);
  smtp.debug(1);
  smtp.callback(smtpCallback);

     GSheet.setTokenCallback(tokenStatusCallback);
    GSheet.setPrerefreshSeconds(10 * 60);

    // Begin the access token generation for Google API authentication
    GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);


  // sendMail("SMARTCARTPARKING","STATUS","SYSTEM READY");
  lcd.clear();
}
 
   bool gate1_sensor_check = true;
   bool gate2_sensor_check = false;
   bool total_space = true;

void loop() {
bool ready = GSheet.ready();

if (total_space){ // close and open totoal space screen;
lcd.setCursor(0,0);
lcd.print("TOTALSPACE: 4");
lcd.setCursor(0,1);
lcd.print("AVAILABLE :     ");
lcd.setCursor(12,1);
lcd.print(car_count);
lcd.setCursor(13,1);
lcd.print("    ");
}


if (gate1_sensor_check) {
  float gate1_detect = check_gate1_sensor();
  if(gate1_detect <=10){
    gate2_sensor_check = false;
    lcd.setCursor(0,0);
    lcd.print("  CAR DETECTED           ");
    scan_rfid = true;
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("PLEASE SCAN CARD       ");
    gate1_scan_rfid();
  } else {
    gate2_sensor_check = true;
  }
}
  
   if (gate2_sensor_check == true){
    float gate2_detect = check_gate2_sensor();
    if(gate2_detect <=10){
    gate1_sensor_check = false;
    total_space = false;
    lcd.setCursor(0,0);
    lcd.print("OUT DETECT       ");
    scan_rfid = true;
    lcd.setCursor(0,1);
    lcd.print("  PLEASE SCAN CARD    ");
    gate2_scan_rfid();
  } else {
    gate1_sensor_check = true;
    total_space = true;
    lcd.clear();
  }
}
}





 


