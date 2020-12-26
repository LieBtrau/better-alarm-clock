#include <Arduino.h>
#include "at24c.h"

AT24C eeprom1(AT24C::AT24C02, 0x50, &Wire);

typedef struct
{
  byte a = 2;
  int b = 0xAA55;
  char str[5] = "hihi";
} CFG;

CFG config;
CFG emtpyConfig={0,0,"0000"};

void printConfig(CFG *cfg)
{
  Serial.println();
  Serial.println(cfg->a, HEX);
  Serial.println(cfg->b, HEX);
  Serial.println(cfg->str);
  Serial.println("##########");
}

void setup()
{
  while (!Serial)
    ;
  Serial.begin(115200);
  Serial.printf("Build %s\r\n", __TIMESTAMP__);
  Wire.begin();
  printConfig(&config);
  if(eeprom1.write(0, config))
  {
    Serial.println("write OK");
  }
  config = emtpyConfig;
  Serial.println("Printing empty config: ");
  printConfig(&config);
  if(eeprom1.read(0, config))
  {
    Serial.println("read ok");
    printConfig(&config);
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.print(".");
  delay(500);
}