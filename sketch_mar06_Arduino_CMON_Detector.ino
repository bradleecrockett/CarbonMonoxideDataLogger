#include <OpenLogManager.h>

#include <SoftwareSerial.h>

// Define the pins for software serial
#define OpenLog_TX_PIN 1 // Arduino RX pin
#define OpenLog_RX_PIN 0 // Arduino TX pin
//#define resetOpenLog 8 // Connect this pin to the OpenLog's DTR or GRN pin to reset it - I don't think I need this? It should auto-reset

const int ledPin = 13;   // LED to tell you when it's reading 
const int mq7Pin = A0;   // Analog pin for the sensor - idk if this is needed either but i'll leave it
int threshold = 300;     // Set threshold (0-1023) based on calibration

SoftwareSerial OpenLog(OpenLog_TX_PIN, OpenLog_RX_PIN); // RX, TX
void setup() 
{
  Serial.begin(9600); // Start serial communication with the computer
  OpenLog.begin(9600); // Start serial communication with OpenLog (default baud rate)

  Serial.println("OpenLog Data Logger Ready");
  // The OpenLog starts logging automatically on power up (indicated by '<')

  Serial.println("Heating... Please wait.");
  delay(20000);          // Allow sensor to preheat for at least 20s to 1 min
}


void loop() 
{
  digitalWrite(ledPin, HIGH);
  int sensorValue = analogRead(mq7Pin); // Read analog value
  Serial.print("CO Value: ");
  Serial.println(sensorValue);

  // Send data to the OpenLog just like the Serial Monitor
  OpenLog.print("Data point: ");
  OpenLog.println(analogRead(A0)); // Log an analog reading

  // Also print to the serial monitor for debugging
  Serial.print("Logged: ");
  Serial.println(analogRead(A0));

  
  delay(2000); // Log every 2 seconds
  digitalWrite(ledPin, LOW);
}

// I don't know if any of this works, there's been so many problems with arduino stuff, hopefully I can test it soon.
