#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
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
uint8_t brightness = 255;
bool led_on = false;

char server_domain_name[] = "http://iot-testing.herokuapp.com";
const int port = 8080;
const int json_capacity = 1000; // Json document capacity
const char* ssid = ""; // Wi-Fi SSID
const char* password =  ""; // Wi-Fi Password

ESP8266WebServer server(port);


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

  send_ip();

  // assign handlers
  server.on("/", handle_index);
  server.on("/update_rgb", handle_update_rgb);
  server.on("/update_brightness", handle_update_brightness);
  server.on("/toggle_led", handle_toggle_led);
  server.on("/sync_data", handle_sync_data);

  // start server
  server.begin();
  Serial.println("server started");
}


void loop() {
  server.handleClient(); //Handle incoming client requests
}


void handle_index() {
  Serial.println("index requested"); // debugging
  server.send(200, "text/plain", "LED Web Server");
}


// handler for POST request to change the RGB color of the LED
void handle_update_rgb() {
  Serial.println("udpate_rgb requested"); // debugging
  // check for POST request body
  if (!server.hasArg("plain")) {
    Serial.println("Error: POST body not found");
    server.send(400, "text/plain", "body not found");
    return;
  }

  // deserialize JSON from POST body
  StaticJsonDocument<json_capacity> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));

  // ensure deserialization was successful
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    server.send(400, "text/plain", "invalid request");
    return;
  }

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
  blue = doc["b"];
  
  update_led();

  server.send(200, "Color has been updated successfully");
}


// handler for POST request to change the LED brightness value
void handle_update_brightness() {
  // check for POST request body
  if (!server.hasArg("plain")) {
    Serial.println("Error: POST body not found");
    server.send(400, "text/plain", "body not found");
    return;
  }

  // deserialize JSON from POST body
  StaticJsonDocument<json_capacity> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));

  // ensure deserialization was successful
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    server.send(400, "text/plain", "invalid request");
    return;
  }
  
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
  
  server.send(200, "Brightness has been updated successfully");
}


// handler for POST request to toggle the LED on or off
void handle_toggle_led() {
  // check for POST request body
  if (!server.hasArg("plain")) {
    Serial.println("Error: POST body not found");
    server.send(400, "text/plain", "body not found");
    return;
  }

  // deserialize JSON from POST body
  StaticJsonDocument<json_capacity> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));

  // ensure deserialization was successful
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    server.send(400, "text/plain", "invalid request");
    return;
  }

  // get JSON object
  JsonObject obj = doc.as<JsonObject>();

  // verify the JSON contains the toggle value
  if (!obj.containsKey("toggle")) {
    Serial.println("error: toggle data missing");
    server.send(400, "invalid request");
    return;
  }
  
  // get toggle value
  int toggle = doc["toggle"];
  led_on = toggle;
  
  update_led();

  server.send(200, "Toggle has been updated successfully");
}


void handle_sync_data(){

int toggleStatus = 0;

if (led_on == false){

  toggleStatus = 0; 

}

else{

  toggleStatus = 1;

}

StaticJsonDocument<1000> message;

message["toggle"] = toggleStatus;

message["red"] = red;

message["green"] = green;

message["blue"] = blue;

message["brightness"] = brightness;

 

String serializedOutput = "";

serializeJson(message, serializedOutput);

//String message = "toggle: " + (String)(led_on) + " colors: " + (String)(red) + ", " + (String)(green) + ", " (String)(blue) + " brightness: " + String(getBrightness());

//String message = "toggle: " + (String)toggleStatus + " colors: " + (String)red + ", " + (String)green + ", " + (String)blue + " brightness: " + (String)brightness;

Serial.println(serializedOutput);

server.send(200, "text/plain", serializedOutput);          //Returns the HTTP response
}


// update the LED color, brightness, and toggle
void update_led() {

  // debugging
  Serial.print("red = ");
  Serial.println(red);
  Serial.print("green = ");
  Serial.println(green);
  Serial.print("blue = ");
  Serial.println(blue);
  Serial.print("brightness = ");
  Serial.println(brightness);
  Serial.print("led_on = ");
  Serial.println(led_on);
  
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


// send public IP address of NodeMCU to the control server
void send_ip() {
  WiFiClient api_client;

  // connect to ipify API
  if (!api_client.connect("api.ipify.org", 80)) {
    Serial.println("failed to connect to ipify");
    return;
  }

  // request plain text IP from API, timeout after 5 seconds
  api_client.print("GET / HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n");
  unsigned long timeout = millis() + 5000;
  while (!api_client.available()) {
    if (millis() > timeout) {
      Serial.println("ipify timed out");
      api_client.stop();
      return;
    }
  }

  // read the API response and parse the IP address string
  String raw_msg = api_client.readString();
  api_client.stop();
  int i = raw_msg.length() - 1;
  while (raw_msg.charAt(i) != '\n') {
    i--;
  }
  String ip_string = raw_msg.substring(i+1);
  Serial.println(ip_string);
  
  HTTPClient client;

  // connect client to the control server with public IP in the address
  String url = server_domain_name + (String) "/api/led_control/client_ip/" + ip_string;
  client.begin(url);
  
  // send GET request and test connection
  int http_code = client.GET();
  if (!client.connected()) {
    Serial.print("could not connect to ");
    Serial.println(url);
  }
  client.end();
  Serial.print("HTTP code from IP send request: ");
  Serial.println(http_code);
}
