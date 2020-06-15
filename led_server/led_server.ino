#include <ESP8266WiFi.h>

// LED pins
#define RED_PIN 4
#define GREEN_PIN 5
#define BLUE_PIN 16

// LED values
uint8_t red = 0;
uint8_t blue = 0;
uint8_t green = 0;
uint8_t brightness = 0;
bool led_on = false;

WiFiServer server(4000);

void setup() {
  // set LED pins to outputs
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  // start serial communicaton at 115200 baud
  Serial.begin(115200);
  Serial.println();

  // ask user for ssid and password of wifi network and connect to that network
  // TODO: use soft AP mode to allow network info to be passed during setup
  Serial.setTimeout(100000);
  Serial.println("Enter network SSID: ");
  String ssid = Serial.readStringUntil('\n');
  Serial.println("Enter network password: ");
  String password = Serial.readStringUntil('\n');
  WiFi.begin(ssid.c_str(), password.c_str());

  // print periods and blink green until connected to the wifi network
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    blink_green();
  }

  // TODO: use api.ipify to print public IP address
  // print the private IP address
  Serial.println();
  Serial.print("Connected, local IP address: ");
  Serial.println(WiFi.localIP());

  
}

void loop() {
  WiFiClient client = server.available();

  // if a client is connected to the server, write to serial and turn the LED red
  if (client) {
    Serial.println("Client connected");
    digitalWrite(RED_PIN, HIGH);

    // read the client's request and print it to the serial monitor
    // TODO: find out how to read entire request from Stream
    while (client.connected()) {
      if (client.available()) {
        String request = client.readString();
        Serial.print(request);
      }
    }

    client.stop();
    Serial.println("Client Disconnected");
    digitalWrite(RED_PIN, LOW);
  }
}

// TODO: implement LED brightness
void update_led() {
  if (led_on) {
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }
}

void blink_green() {
  digitalWrite(GREEN_PIN, HIGH);
  delay(100);
  digitalWrite(GREEN_PIN, LOW);
}

//  for (int i = 0; i < 256; i++) {
//    analogWrite(BLUE_PIN, i);
//    delay(10);
//  }
//  for (int i = 0; i < 256; i++) {
//    analogWrite(BLUE_PIN, 255-i);
//    delay(10);
//  }
