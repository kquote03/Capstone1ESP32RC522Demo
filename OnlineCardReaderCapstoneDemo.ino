/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>
#include <MFRC522.h>
#include <SPI.h>


#define RST_PIN         32          // Configurable, see typical pin layout above
#define SS_PIN          33         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#endif

// Set these to your desired credentials.
const char *ssid = "CapstoneDemo-TestESP32";
const char *password = "12345678";

NetworkServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  SPI.begin();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while (1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

  Serial.println("Initializing RC522");
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println("RC522 Initialized.");

}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            //client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            //client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
            client.print("<h1>AAU Capstone 1 ESP32 Mifare Reading Demo</h1><br/><h3>No Mifare cards were harmed in the making of this demo [yet.... ;) ]</h3><br/><b>Please place the card in front of the reader after selecting one of the functions</b><br/>");
            client.print("<button onclick='window.location=\"/H\"'>Dump Card Details</button>");
            client.print("<button onclick='window.location=\"/L\"'>Dump Card Contents</button>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          while(true){
           	if ( mfrc522.PICC_IsNewCardPresent()) {
           		break;
             }
          }
          while(true){
            if ( mfrc522.PICC_ReadCardSerial()) {
		          break;
	          }
          }
          //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          client.println("CARD READ: ");
          client.print(mfrc522.PICC_SaveCardDetails(&(mfrc522.uid)));
          client.println();

          //Serial.println(mfrc522.s)
          digitalWrite(LED_BUILTIN, HIGH);  // GET /H turns the LED on
        }

        if (currentLine.endsWith("GET /L")) {
          while(true){
           	if ( mfrc522.PICC_IsNewCardPresent()) {
           		break;
             }
          }
          while(true){
            if ( mfrc522.PICC_ReadCardSerial()) {
		          break;
	          }
          }
          //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          client.println("CARD READ: ");
          client.print(mfrc522.PICC_SaveToCharArray(&(mfrc522.uid)));
          client.println();

          //Serial.println(mfrc522.s)
          digitalWrite(LED_BUILTIN, HIGH);  // GET /H turns the LED on
          digitalWrite(LED_BUILTIN, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
