 #include <WiFi.h>
#include <HTTPClient.h>

#define FLAME_SENSOR_PIN 32  // Replace 'X' with the analog input pin connected to the flame sensor
#define LED_PIN 23           // Replace 'X' with the GPIO number where your LED is connected

const char* ssid = "TOPNET_D148";
const char* password = "ghostytn@2022";
const char* TWILIO_ACCOUNT_SID = "AC3503029d11966f6b80838dd11bf09bff";
const char* TWILIO_AUTH_TOKEN = "32a2b32acfbc5c2bf3c925f54d6942b9";
const char* TWILIO_PHONE_NUMBER = "623006033";
const char* YOUR_PHONE_NUMBER = "58113274";

void setup() {
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  connectToWiFi();
}

void loop() {
  int sensorValue = analogRead(FLAME_SENSOR_PIN);
  int threshold = 500; // Adjust the threshold based on your specific setup

  if (sensorValue > threshold) {
    digitalWrite(LED_PIN, HIGH); // Turn on the LED if the sensor detects a flame
    Serial.println("Flame detected!");

    // Send SMS notification
    String message = "Flame detected! Take necessary action.";
    if (sendSMS(message)) {
      Serial.println("SMS sent successfully.");
    } else {
      Serial.println("SMS sending failed.");
    }
  } else {
    digitalWrite(LED_PIN, LOW); // Turn off the LED if the sensor does not detect a flame
    Serial.println("No flame detected.");
  }
  delay(500); // You can adjust this delay to control how often the sensor is read
}

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
}

bool sendSMS(const String& message) {
  WiFiClientSecure client;
  client.setTimeout(10000);

  HTTPClient http;
  http.begin(client, "https://api.twilio.com/2010-04-01/Accounts/" + String(TWILIO_ACCOUNT_SID) + "/Messages.json");
  http.setAuthorization(TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String postData = "To=" + String(YOUR_PHONE_NUMBER) + "&From=" + String(TWILIO_PHONE_NUMBER) + "&Body=" + message;
  int httpResponseCode = http.POST(postData);

  http.end();

  return (httpResponseCode == 201); // Twilio returns 201 for successful SMS sending
}
