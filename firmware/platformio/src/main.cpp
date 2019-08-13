/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"
#include "parameters.h"
#include "EEPROMAnything.h"

typedef struct
{
  CommonConfig commConfig;
  AlarmConfig alarmConfig1;
  AlarmConfig alarmConfig2;
} EepromConfig;

void assignAlarmConfig(byte nr);
EepromConfig config;
static EepromConfig readConfig;
extern ActionMgr actionMgr;
extern MenuMgr menuMgr;
HardwareSerial *ser1 = &Serial;

void setup()
{
  while (!*ser1)
    ;
  ser1->begin(115200);
  if (EEPROM_readAnything(0, readConfig))
  {
    ser1->println("Using config from EEPROM.");
    memcpy(&config, &readConfig, sizeof(config));
  }
  menuMgr.assignCommonConfig(&config.commConfig);
  actionMgr.assignCommonConfig(&config.commConfig);
  actionMgr.updateAlarmSettings(&config.alarmConfig1, 0);
  actionMgr.updateAlarmSettings(&config.alarmConfig2, 1);
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
  actionMgr.pollActions();
  menuMgr.pollMenu();
  menuMgr.setBrightness(actionMgr.isDark() ? config.commConfig.nightBright : config.commConfig.dayBright);
  menuMgr.showParameterMenu(true);
}

void assignAlarmConfig(byte nr)
{
  switch (nr)
  {
  case 1:
    menuMgr.assignAlarmConfig(&config.alarmConfig1);
    actionMgr.assignAlarmConfig(0, &config.alarmConfig1);
    break;
  case 2:
    menuMgr.assignAlarmConfig(&config.alarmConfig2);
    actionMgr.assignAlarmConfig(1, &config.alarmConfig2);
    break;
  default:
    return;
  }
}

void saveConfig()
{
  actionMgr.updateAlarmSettings(&config.alarmConfig1, 0);
  actionMgr.updateAlarmSettings(&config.alarmConfig2, 1);
  // EEPROM_writeAnything(0, config);
}