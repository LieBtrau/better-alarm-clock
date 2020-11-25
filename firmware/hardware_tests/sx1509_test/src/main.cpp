/*************************************************************
keypadInterrupt.ino
SparkFun SX1509 I/O Expander Example: keypad matrix with int
Jim Lindblom @ SparkFun Electronics
Original Creation Date: September 21, 2015
https://github.com/sparkfun/SparkFun_SX1509_Arduino_Library

This example demonstrates how to use the SX1509's keypad
engine to monitor a matrix of button inputs. The SX1509's
interrupt output is monitored to check for button presses.

For this example, we use the 12-button keypad
  (https://www.sparkfun.com/products/8653).

After uploading the sketch, open your serial monitor and 
set it to 9600 baud.

Hardware Hookup:
	SX1509 Breakout ------ Arduino -------- Keypad Pin
	      INT --------------- D2
	      GND -------------- GND
	      3V3 -------------- 3.3V
		  SDA ------------ SDA (A4)
		  SCL ------------ SCL (A5)
		  0 ---------------------------------- 2 (row 1)
		  1 ---------------------------------- 7 (row 2)
		  2 ---------------------------------- 6 (row 3)
		  3 ---------------------------------- 4 (row 4)
		  8 ---------------------------------- 3 (col 1)
		  9 ---------------------------------- 1 (col 2)
		  10 --------------------------------- 5 (col 3)

Development environment specifics:
	IDE: Arduino 1.6.5
	Hardware Platform: Arduino Uno
	SX1509 Breakout Version: v2.0

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, 
please buy us a round!

Distributed as-is; no warranty is given.
*************************************************************/

#include <Wire.h>           // Include the I2C library (required)
#include <SparkFunSX1509.h> // Include SX1509 library

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address
SX1509 io1, io2;                      // Create an SX1509 object to be used throughout

#define KEY_ROWS 4 // Number of rows in the keypad matrix
#define KEY_COLS 2 // Number of columns in the keypad matrix

// keyMap maps row/column combinations to characters:
char* io1_keyMap[KEY_ROWS][KEY_COLS] = {
    {"Mon", "Fri"},
    {"Tue", "Sat"},
    {"Wed", "Sun"},
    {"Thu", "No"}};
char* io2_keyMap[KEY_ROWS]={"Light", "Volume", "Song", "AlarmTime"};

const byte ARDUINO_INTERRUPT_PIN = PB11;

void setup()
{
  // Serial is used to display the keypad button pressed:
  Serial.begin(115200);
  // Call io.begin(<address>) to initialize the SX1509. If it
  // successfully communicates, it'll return 1.
  if (!io1.begin(IO1_SX1509_ADDRESS))
  {
    Serial.println("Failed to communicate with io1");
    while (1)
      ; // If we fail to communicate, loop forever.
  }
  if (!io2.begin(IO2_SX1509_ADDRESS))
  {
    Serial.println("Failed to communicate with io2");
    while (1)
      ; // If we fail to communicate, loop forever.
  }
  Serial.printf("Build %s\r\n", __TIMESTAMP__);

  // Use the internal 2MHz oscillator.
  // Set LED clock to 500kHz (2MHz / (2^(3-1)):
  io1.clock(INTERNAL_CLOCK_2MHZ, 3);
  io2.clock(INTERNAL_CLOCK_2MHZ, 3);

  // Scan time range: 1-128 ms, powers of 2
  byte scanTime = 16; // Scan time per row, in ms
  // Debounce time range: 0.5 - 64 ms (powers of 2)
  byte debounceTime = 8; // Debounce time
  // Sleep time range: 128 ms - 8192 ms (powers of 2) 0=OFF
  byte sleepTime = 128;
  // Scan time must be greater than debounce time!
  io1.keypad(KEY_ROWS, KEY_COLS, sleepTime, scanTime, debounceTime);
  io2.keypad(KEY_ROWS, 1, sleepTime, scanTime, debounceTime);

  io1.pinMode(4, OUTPUT); // Set LED pin to OUTPUT
  io1.pinMode(5, ANALOG_OUTPUT);
  ; // Set LED pin to OUTPUT
  io1.pinMode(6, ANALOG_OUTPUT);
  io1.analogWrite(5, 20);
  io1.pinMode(7, OUTPUT);
  io1.pinMode(12, ANALOG_OUTPUT);
  io1.pinMode(13, ANALOG_OUTPUT);
  io1.pinMode(14, ANALOG_OUTPUT);
  io1.pinMode(15, ANALOG_OUTPUT);

  // Blink the LED pin -- ~1000 ms LOW, ~500 ms HIGH:
  io1.blink(4, 1000, 500);
  io1.breathe(6, 1000, 500, 500, 250);
  io1.digitalWrite(6, LOW);
  io1.digitalWrite(7, LOW);
  io1.breathe(12, 1000, 500, 500, 250);
  io1.breathe(13, 1000, 500, 1000, 250);
  io1.breathe(14, 1000, 500, 750, 250);
  io1.breathe(15, 1000, 500, 1250, 250);

  io2.pinMode(4, ANALOG_OUTPUT);
  io2.pinMode(5, ANALOG_OUTPUT);
  io2.pinMode(6, ANALOG_OUTPUT);
  io2.pinMode(7, ANALOG_OUTPUT);
  io2.breathe(4, 1000, 500, 500, 250);
  io2.breathe(5, 1000, 500, 1000, 250);
  io2.breathe(6, 1000, 500, 750, 250);
  io2.breathe(7, 1000, 500, 1250, 250);


  // Set up the Arduino interrupt pin as an input w/
  // internal pull-up. (The SX1509 interrupt is active-low.)
  pinMode(ARDUINO_INTERRUPT_PIN, INPUT_PULLUP);
}

// Compared to the keypad in keypad.ino, this keypad example
// is a bit more advanced. We'll use these varaibles to check
// if a key is being held down, or has been released. Then we
// can kind of emulate the operation of a computer keyboard.
unsigned int previousKeyData = 0;         // Stores last key pressed
unsigned int holdCount, releaseCount = 0; // Count durations
const unsigned int holdCountMax = 15;     // Key hold limit
const unsigned int releaseCountMax = 100; // Release limit

void loop()
{
  unsigned int keyData;
  byte row, col;
  char* key;

  // If the SX1509 INT pin goes low, a keypad button has
  // been pressed:
  if (digitalRead(ARDUINO_INTERRUPT_PIN) == LOW)
  {
     // Use io.readKeypad() to get the raw keypad row/column
    keyData = io1.readKeypad();
    if (keyData)
    {
      // Then use io.getRow() and io.getCol() to parse that
      // data into row and column values.
      row = io1.getRow(keyData);
      col = io1.getCol(keyData);
     // Then plug row and column into keyMap to get which
    // key was pressed.
    key = io1_keyMap[row][col];
   }
    else
    {
      keyData = io2.readKeypad();
      row = io2.getRow(keyData);
      key = io2_keyMap[row];
    }


    // If it's a new key pressed
    if (keyData != previousKeyData)
    {
      holdCount = 0;               // Reset hold-down count
      Serial.println(key); // Print the key
    }
    else // If the button's beging held down:
    {
      holdCount++;                  // Increment holdCount
      if (holdCount > holdCountMax) // If it exceeds threshold
        Serial.println(key);        // Print the key
    }
    releaseCount = 0;          // Clear the releaseCount variable
    previousKeyData = keyData; // Update previousKeyData
  }

  // If no keys have been pressed we'll continuously increment
  //  releaseCount. Eventually creating a release, once the
  // count hits the max.
  releaseCount++;
  if (releaseCount >= releaseCountMax)
  {
    releaseCount = 0;
    previousKeyData = 0;
  }
  delay(1); // Gives releaseCountMax a more intuitive unit
}
