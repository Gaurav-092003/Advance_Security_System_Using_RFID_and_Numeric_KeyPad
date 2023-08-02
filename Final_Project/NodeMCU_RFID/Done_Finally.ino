#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 2
#define RST_PIN D0
#define relayInput D3 //relay pin
#define ACCESS_DELAY 5000
#define DENIED_DELAY 1000
MFRC522 mfrc522(SS_PIN, RST_PIN);


#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

#define WIFI_SSID "Nahi milega internet."
#define WIFI_PASSWORD "Gaurav0909*"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "edi.sygroup4@gmail.com"
#define AUTHOR_PASSWORD "qvelugfxvxqvepij"

/* Recipient's email*/
#define RECIPIENT_EMAIL "gaurav.bhole21@vit.edu"

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init(); 
  pinMode(relayInput, OUTPUT);
  digitalWrite(relayInput, HIGH);
  Serial.println("Put your card to the reader...");
  Serial.println();



//  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();



}

void loop() {
  // put your main code here, to run repeatedly:
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "63 2C 52 16") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    delay(500);
    digitalWrite(relayInput, LOW);
    delay(ACCESS_DELAY);
    digitalWrite(relayInput, HIGH);
  }
  else{
    Serial.println(" Access denied");
    smtp.debug(1);
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";
  SMTP_Message message;
  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP Test Email";
  message.addRecipient("Sara", RECIPIENT_EMAIL);
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Message From Advance Door Lock System</h1><p>Some one is trying to acess your house</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  
  if (!smtp.connect(&session))
    return;

  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
  }
}
