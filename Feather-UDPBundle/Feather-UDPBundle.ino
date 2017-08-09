/*---------------------------------------------------------------------------------------------

  Open Sound Control (OSC) library for the ESP8266

  Example for receiving open sound control (OSC) bundles on the ESP8266
  Send integers '0' or '1' to the address "/led" to turn on/off the built-in LED of the esp8266.

  This example code is in the public domain.

  --------------------------------------------------------------------------------------------- */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

char ssid[] = "InspirationHub";          // your network SSID (name)
char pass[] = "Inspiration";                    // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const unsigned int localPort = 2390;        // local port to listen for UDP packets (here's where we send the packets)

#define CTRL_LED 0

OSCErrorCode error;
unsigned int receivedPosition = 0;              // LOW means led is *on*

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  pinMode(CTRL_LED, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    errorBlink(50);
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());


}

void errorBlink(unsigned int delayMs) {
  digitalWrite(CTRL_LED, HIGH);
  delay(delayMs / 2);
  digitalWrite(CTRL_LED, LOW);
  delay(delayMs / 2);
}

void positionChange(OSCMessage &msg) {

  receivedPosition = 255 * msg.getFloat(0);
  analogWrite(CTRL_LED, receivedPosition);
  
  Serial.print("/position: ");
  Serial.println(receivedPosition);

}

void loop() {

  // Control the connection (led #0)
  if (WiFi.status() != WL_CONNECTED) {
    // not connected => Message + Blink Short
    Serial.println("Wifi Not Connected :(");
    errorBlink(50);

  } else {
    
    // Then wait for OSC
    OSCBundle bundle;
    int size = Udp.parsePacket();

    if (size > 0) {
      Serial.println("OSC Packet Received");

      while (size--) {
        bundle.fill(Udp.read());
      }

      if (!bundle.hasError()) {
        bundle.dispatch("/position", positionChange);
      } else {
        error = bundle.getError();
        Serial.print("error: ");
        Serial.println(error);
        // not connected => Message + Blink Lon
        errorBlink(100);

      }
    }
  }
}


