#include <ESP8266WiFi.h>

#define RED_PIN 4
#define GREEN_PIN 5
#define BLUE_PIN 16

const char* host = "";
WiFiClient client;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();

  WiFi.begin("", "");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    blinkGreen();
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.printf("\nConnecting to %s ... ", host);
  if (client.connect(host, 80)) {
    Serial.println("connected");
    
    Serial.println("Sending request");
    client.print(request);

    Serial.println("Response: ");
    while (client.connected || client.available()) {
      if (client.available()) Serial.println(client.readStringUntil('\n'));
    }

    client.stop();
    Serial.println("\nDisconnected");
  }
  
  else {
    Serial.println("Connection Failed");
    client.stop();
  }

  delay(5000);
}

void blinkGreen() {
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
