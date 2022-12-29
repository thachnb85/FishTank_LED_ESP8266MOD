/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/

#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "credential.h"
#include <NTPClient.h>
#include <ESP8266HTTPUpdateServer.h>

#define PIN D4
#define serialRate 9600
#define NUM_LEDS 25

// Create credential.h file with below line
// #define SID "Your SSID"
// #define PW "You WIFI PW"

// Manual turn on or off
bool isManual = false;

// LED String
Adafruit_NeoPixel LEDs(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

// NTP Server to get time
// How to calculate offset:

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// Define NTP Client to get time
// Kelowna BC GMT-8
// -8 * 60 * 60 = -28800
const long utcOffsetInSeconds = -28800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void controlLed(int val) {
  if (val == 0) {
    switchOffLED();
  } else if (val == 1) {
    switchOnLED();
  }
}

void switchOnLED() {  
  for (int i = 0; i < NUM_LEDS; i++)
  {
    LEDs.setPixelColor(i, LEDs.Color(255, 255, 246));
  }
  LEDs.show();
}

void switchOffLED() {
  for (int i = 0; i < NUM_LEDS; i++)
  {
    LEDs.setPixelColor(i, LEDs.Color(0, 0, 0));
  }
  LEDs.show();
}

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {

  Serial.begin(serialRate);

  delay(500);
  LEDs.begin();

  pinMode(BUILTIN_LED, OUTPUT);

  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SID, PW);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  timeClient.begin();
}

void loop() {
  timeClient.update();

  int currentHour = timeClient.getHours();
  int currentMin = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  // Switch off at night
  if (isManual == false) {
    if (currentHour >= 21 || currentHour <= 6) {
    switchOffLED();
    } else {
      switchOnLED();
    }
  }

  delay(500);

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(100);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  String s; // response
  
  if (req.indexOf("/led/off") != -1) {
    Serial.println("Requesting LED OFF");
    val = 0;
    isManual = true;
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\r\n\r\nLED is now OFF";
  } else if (req.indexOf("/led/on") != -1) {
    Serial.println("Requesting LED ON");
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\r\n\r\nLED is now ON";
    val = 1;
    isManual = true;
  } else if (req.indexOf("/led/auto") != -1) {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\r\n\r\nLED is now AUTO";
    Serial.println("Reset manual");
    isManual = false;
  } else {
    Serial.println("INVALID REQUEST");
    client.stop();
    return;
  }

  controlLed(val);

  client.flush();

  // Send the response to the client
  client.print(s);
  delay(100);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
