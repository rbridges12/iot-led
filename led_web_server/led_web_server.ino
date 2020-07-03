#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// LED pins
#define RED_PIN 4
#define GREEN_PIN 5
#define BLUE_PIN 16
#define STATUS_PIN LED_BUILTIN

// LED values
uint8_t red = 0;
uint8_t blue = 0;
uint8_t green = 0;
uint8_t brightness = 0;
bool led_on = false;

ESP8266WebServer server(8080);

const int json_capacity = 100; // Json document capacity
const char* ssid = ""; // Wi-Fi SSID
const char* password =  ""; // Wi-Fi Password

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(STATUS_PIN, OUTPUT);

  // connect to serial and WiFi
  Serial.begin(115200);
  WiFi.begin(ssid, password); 

 // wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
      delay(400);
      Serial.print(".");
      blink_status();
  }

  // print local IP
  Serial.println();
  Serial.print("Connected, local IP address: ");
  Serial.println(WiFi.localIP());

  // set handlers
  server.on("/", handle_index);
  server.on("/update_light", handle_update_light);

  // start server
  server.begin();
  Serial.println("server started");
}

void loop() {
  server.handleClient(); //Handle incoming client requests
}

void handle_index() {
  server.send(200, "text/plain", "LED Web Server");
}

void handle_update_light() {

  // check for POST request body
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "body not found");
    return;
  }

  // deserialize JSON from POST body
  StaticJsonDocument<json_capacity> settings;
  DeserializationError err = deserializeJson(settings, server.arg("plain"));

  // ensure deserialization was successful
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    server.send(400, "text/plain", "invalid request");
    return;
  }

  // get led settings from JSON
  // TODO: ensure all values were read correctly
  red = settings["r"];
  green = settings["g"];
  blue = settings["b"];
  brightness = settings["brightness"];
  led_on = settings["toggle"];

  update_led();
}

void update_led() {
  if (led_on) {
    float brightness_factor = brightness / 255.0;
    analogWrite(RED_PIN, red * brightness_factor);
    analogWrite(GREEN_PIN, green * brightness_factor);
    analogWrite(BLUE_PIN, blue * brightness_factor);
  }
  else {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
}

void blink_status() {
  digitalWrite(STATUS_PIN, HIGH);
  delay(100);
  digitalWrite(STATUS_PIN, LOW);
}
