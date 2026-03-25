//this is mainly cobbled together from example programs
//The Arduino I'm using seems to not be able to handle multiple serial connections, so I need one that can, and then this should work.
//There's probably some stuff in here that I still need to fix, but I'm putting it in GitHub just in case.

//#include "SparkFun_Si7021_Breakout_Library.h"
#include <Wire.h>
#include <SoftwareSerial.h>
#include <OpenLogManager.h>
#include "SparkFun_SHTC3.h" // Click here to get the library: http://librarymanager/All#SparkFun_SHTC3

SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class
// Define the pins for software serial
#define OpenLog_TX_PIN 1 // Arduino RX pin
#define OpenLog_RX_PIN 0 // Arduino TX pin

// RX pin 0, TX pin 1 (Connect to OpenLog TX and RX, Arduino TX to Openlog RX and Arduino RX to Openlog TX)
SoftwareSerial openLogSerial(0, 1);
// Initialize the manager passing the serial stream
OpenLogManager myLogger(openLogSerial);
//#define resetOpenLog 8 // Connect this pin to the OpenLog's DTR or GRN pin to reset it - I don't think I need this? It should auto-reset
float humidity = 0;
float tempf = 0;
const int ledPin = 13;   // LED to tell you when it's reading 
const int mq7Pin = A0;   // Analog pin for the sensor - idk if this is needed either but i'll leave it
int threshold = 300;     // Set threshold (0-1023) based on calibration


//SoftwareSerial OpenLog(OpenLog_TX_PIN, OpenLog_RX_PIN); // RX, TX
void setup() 
{
  
  Serial.begin(9600); // Start serial communication with t/he computer
  while(Serial == false){};
  Serial.print("5 seconds to check connections.");
  delay(5000);
  openLogSerial.begin(9600); // Communication with OpenLog
  delay(2000);

  Serial.println(F("Starting OpenLog Manager..."));

  // 1. Enter Command Mode
  if (myLogger.enterCommandMode()) {
    Serial.println(F("Entered Command Mode successfully."));

    // 2. Find the index of the last file (e.g., if log3.txt exists, returns 3)
    int lastIndex = myLogger.getLastFileNumber("log");
    int newIndex = lastIndex + 1;

    Serial.print(F("Last log index: "));
    Serial.println(lastIndex);
    Serial.print(F("Creating new file: log"));
    Serial.print(newIndex);
    Serial.println(F(".txt"));

    // 3. Create the new file
    if (myLogger.createNewFile("log", newIndex)) {
      Serial.println(F("File created."));
      
      // 4. Start appending data to the file
      if (myLogger.appendToFile("log" + String(newIndex) + ".txt")) {
        Serial.println(F("Ready to log data."));
      }
    } else {
      Serial.println(F("Error creating file."));
    }
  } else {
    Serial.println(F("Error: OpenLog not responding. Check connections."));
  }    
  //sensorSerial.begin(115200);//Serial.begin(115200); // Begin Serial
  //while(Serial == false){};
  //OpenLog.begin(9600); // Start serial communication with OpenLog (default baud rate)

  Serial.println("Heating... Please wait.");
  delay(20000);          // Allow sensor to preheat for at least 20s to 1 min
 
  Wire.begin();
  Serial.print("Beginning temp/humidity sensor. Result = ");           // Most SHTC3 functions return a variable of the type "SHTC3_Status_TypeDef" to indicate the status of their execution 
  errorDecoder(mySHTC3.begin());                              // To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
  Serial.println();
  Serial.println("\n\n");
  Serial.println("Waiting for 5 seconds so you can read this info ^^^");

  delay(5000);                                            // Give time to read the welcome message and device ID. 
}


void loop() 
{
  SHTC3_Status_TypeDef result = mySHTC3.update();             // Call "update()" to command a measurement, wait for measurement to complete, and update the RH and T members of the object
  printInfo();                                                // This function is used to print a nice little line of info to the serial port
  delay(190);                                                 // Delay for the data rate you want - note that measurements take ~10 ms so the fastest data rate is 100 Hz (when no delay is used)

  digitalWrite(ledPin, HIGH);
  int sensorValue = analogRead(mq7Pin); // Read analog value
  Serial.print("CO Value: ");
  Serial.println(sensorValue);

  // Send data to the OpenLog just like you would to the Serial Monitor
  //OpenLog.print("Data point: ");
  //OpenLog.println(analogRead(A0)); // Log an analog reading

  // Also print to the serial monitor for debugging
  Serial.print("Logged: ");
  Serial.println(analogRead(A0));
  // Write data periodically
  String dataLine = "Time: " + String(millis()) + " ms | CO Value: " + String(sensorValue) + "\r\n";
  // Blink LED to indicate activity
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(2000); // Log every 2 seconds
  digitalWrite(ledPin, LOW);
}

///////////////////////
// Utility Functions //
///////////////////////
void printInfo()
{
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)              // You can also assess the status of the last command by checking the ".lastStatus" member of the object
  {
    Serial.print("RH = "); 
    Serial.print(mySHTC3.toPercent());                        // "toPercent" returns the percent humidity as a floating point number
    Serial.print("%, T = "); 
    Serial.print(mySHTC3.toDegF());                           // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively 
    Serial.println(" deg F"); 
  }
  else
  {
    Serial.print("Update failed, error: "); 
    errorDecoder(mySHTC3.lastStatus);
    Serial.println();
  }
}

void errorDecoder(SHTC3_Status_TypeDef message)                             // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch(message)
  {
    case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
    case SHTC3_Status_Error : Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
    default : Serial.print("Unknown return code"); break;
  }
}
