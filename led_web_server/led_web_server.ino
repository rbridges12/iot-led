#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(8080);

const char* ssid = ""; //Enter Wi-Fi SSID
const char* password =  ""; //Enter Wi-Fi Password
const int pin = LED_BUILTIN;
int newRequestComing = 0;
 
void setup() {
  newRequestComing = 0;
  pinMode(pin, OUTPUT);
  //pinMode(pin, LOW);
  Serial.begin(115200); //Begin Serial at 115200 Baud
  WiFi.begin(ssid, password);  //Connect to the WiFi network
 
  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection
      delay(500);
      Serial.println("Waiting to connect...");
  }
 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP
 
  server.on("/", handle_index); //Handle Index page
  server.on("/lighton", handle_lighton);
  server.on("/lightoff", handle_lightoff);
  server.on("/lightblink",handle_lightblink);
 
  server.begin(); //Start the server
  Serial.println("Server listening");
}

void loop() {
  server.handleClient(); //Handling of incoming client requests
}

void handle_index() {
  //Print Hello at opening homepage
  server.send(200, "text/plain", "Hello! This is an index page.");
}
void handle_lightoff() {
  newRequestComing=1;
  Serial.println("newRequestComing="+newRequestComing);
  digitalWrite(pin, HIGH);
  Serial.println("LED ON");
  server.send(200, "text/plain", "Light is turned on.");
  newRequestComing = 0;
}
void handle_lighton() {
 
  newRequestComing=1;
  Serial.println("newRequestComing="+newRequestComing);
  digitalWrite(pin, LOW);
  Serial.println("LED OFF");
  server.send(200, "text/plain", "Light is turned off.");
  newRequestComing = 0;
}
void handle_lightblink() {
  //newRequestComing = false;
  int count = 0;
  server.send(200, "text/plain", "Light is blinking for 10 times");
  do {
    count++;
   Serial.println("newRequestComing="+newRequestComing);
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on by making the voltage LOW
    delay(500);                      // Wait for a second
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    delay(500);
  } while (count < 10);

}
