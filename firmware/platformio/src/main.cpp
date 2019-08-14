#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"
#include "parameters.h"
#include "at24c.h"

typedef struct
{
  CommonConfig commConfig;
  AlarmConfig alarmConfig[MAX_ALARMS];
} EepromConfig;

void assignAlarmConfig(ALARMNRS nr);
void updateAlarmSettings();
EepromConfig config;
static EepromConfig readConfig;
extern ActionMgr actionMgr;
extern MenuMgr menuMgr;
HardwareSerial *ser1 = &Serial;
AT24C eeprom1(AT24C::AT24C02, 0x50, &Wire);
const byte CFG_MEM_ADDRESS = 0;

void setup()
{
  while (!*ser1)
    ;
  ser1->begin(115200);
  Wire.begin();
  if (eeprom1.read(CFG_MEM_ADDRESS, readConfig))
  {
    ser1->println("Using config from EEPROM.");
    memcpy(&config, &readConfig, sizeof(EepromConfig));
  }
  menuMgr.assignCommonConfig(&config.commConfig);
  actionMgr.assignCommonConfig(&config.commConfig);
  updateAlarmSettings();
  if (!actionMgr.initPeripherals())
  {
    ser1->println("Can't init peripherals");
    while (true)
      ;
  }
  menuMgr.initMenu(actionMgr.getTotalTrackCount());
  menuMgr.showSplash();
  showClock(true);
}

void loop()
{
  byte keyPressed = menuMgr.pollMenu();
  actionMgr.pollActions(keyPressed);
  menuMgr.setBrightness(actionMgr.isDark() ? config.commConfig.nightBright : config.commConfig.dayBright);
  menuMgr.showParameterMenu(true);
}

void assignAlarmConfig(ALARMNRS nr)
{
  if (nr < MAX_ALARMS)
  {
    menuMgr.assignAlarmConfig(&config.alarmConfig[nr]);
    actionMgr.assignAlarmConfig(nr, &config.alarmConfig[nr]);
  }
}

void saveConfig()
{
  updateAlarmSettings();
  if (memcmp(&config, &readConfig, sizeof(EepromConfig)))
  {
    Serial.println("writing config");
    if (!eeprom1.write(CFG_MEM_ADDRESS, config))
    {
      Serial.println("Can't write eeprom.");
    }
  }
}

void updateAlarmSettings()
{
  actionMgr.updateAlarmSettings(&config.alarmConfig[0], ALARM1);
  actionMgr.updateAlarmSettings(&config.alarmConfig[1], ALARM2);
}