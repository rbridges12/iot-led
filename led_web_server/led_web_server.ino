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

  // assign handlers
  server.on("/", handle_index);
  server.on("/update_rgb", handle_update_rgb);
  server.on("/update_brightness", handle_update_brightness);
  server.on("/toggle_led", handle_toggle_led);

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


// handler for POST request to change the RGB color of the LED
void handle_update_rgb() {
  StaticJsonDocument<> doc = verify_json();

  // return if verify_json was unsuccessful
  if (!doc) return;

  // get JSON object
  JsonObject obj = doc.as<JsonObject>();

  // verify the JSON contains the RGB values
  if (!obj.containsKey("r") ||
      !obj.containsKey("g") ||
      !obj.containsKey("b")) {
        Serial.println("Error: missing RGB value(s)");
        server.send(400, "text/plain", "invalid request");
        return;
  }
      
  // get RGB values and update LED
  red = doc["r"];
  green = doc["g"];
  blue = doc"b"];
  
  update_led();
}


// handler for POST request to change the LED brightness value
void handle_update_brightness() {
  StaticJsonDocument<> doc = verify_json();

  // return if verify_json was unsuccessful
  if (!doc) return;

  // get JSON object
  JsonObject obj = doc.as<JsonObject>();

  // verify the JSON contains the brightness value
  if (!obj.containsKey("brightness")) {
    Serial.println("Error: brightness data missing");
    server.send(400, "invalid request");
    return;
  }

  // get the brightness value
  brightness = doc["brightness"];
  
  update_led();
}


// handler for POST request to toggle the LED on or off
void handle_toggle_led() {
  StaticJsonDocument<> doc = verify_json();

  // return if verify_json was unsuccessful
  if (!doc) return;

  // get JSON object
  JsonObject obj = doc.as<JsonObject>();

  // verify the JSON contains the toggle value
  if (!obj.containsKey("toggle")) {
    Serial.println("error: toggle data missing");
    server.send(400, "invalid request");
    return;
  }
  
  // get toggle value
  led_on = doc["toggle"];
  
  update_led();
}


// verify POST request and the JSON contained in it
// returns the JSON document or null if there were any errors
StaticJsonDocument<json_capacity> verify_json() {
  // check for POST request body
  if (!server.hasArg("plain")) {
    Serial.println("Error: POST body not found");
    server.send(400, "text/plain", "body not found");
    return null;
  }

  // deserialize JSON from POST body
  StaticJsonDocument<json_capacity> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));

  // ensure deserialization was successful
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    server.send(400, "text/plain", "invalid request");
    return null;
  }

  return doc;
}


// update the LED color, brightness, and toggle
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


// blink the status light
void blink_status() {
  digitalWrite(STATUS_PIN, HIGH);
  delay(100);
  digitalWrite(STATUS_PIN, LOW);
}