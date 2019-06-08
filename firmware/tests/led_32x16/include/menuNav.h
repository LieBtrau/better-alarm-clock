#pragma once
#include "Max72xxPanel.h"
#include "Fonts/Picopixel.h"
#include "Fonts/TomThumb.h"
#include <SPI.h>
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Menu.h"
#include "Adafruit_MCP23017.h"
#include "KeyboardScan.h"

 void initMenu();
 void animation();
 void renderMenu();
