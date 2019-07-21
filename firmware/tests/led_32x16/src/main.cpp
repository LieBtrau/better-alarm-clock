/* https://hackaday.com/2015/09/04/embed-with-elliot-practical-state-machines/
 */
#include "Arduino.h"
#include "menuNav.h"
#include "actions.h"
#include "parameters.h"
#include "SongPlayer.h"
#include "EEPROMAnything.h"

typedef struct
{
  CommonConfig commConfig;
  AlarmConfig alarmConfig1;
  AlarmConfig alarmConfig2;
} EepromConfig;

void getAlarmConfig(byte nr);
EepromConfig config;
extern ActionMgr actionMgr;
extern MenuMgr menuMgr;

void setup()
{
  Serial.begin(115200);
  EepromConfig tempConfig;
  if (EEPROM_readAnything(0, tempConfig))
  {
    Serial.println("Using config from EEPROM.");
    memcpy(&config, &tempConfig, sizeof(config));
  }
  menuMgr.assignCommonConfig(&config.commConfig);
  actionMgr.assignCommonConfig(&config.commConfig);
  if (!actionMgr.initPeripherals())
  {
    Serial.println("Can't init peripherals");
    while (true)
      ;
  }
  initMenu(actionMgr.getTotalTrackCount());
  //EEPROM_writeAnything(0, config1);
  showSplash();
  showClock(true);
}

void loop()
{
  actionMgr.pollActions();
  bool flashing = pollMenu();
  showParameterMenu(flashing);
}

void getAlarmConfig(byte nr)
{
  switch (nr)
  {
  case 1:
    menuMgr.assignAlarmConfig(&config.alarmConfig1);
    actionMgr.assignAlarmConfig(&config.alarmConfig1);
    break;
  case 2:
    menuMgr.assignAlarmConfig(&config.alarmConfig2);
    actionMgr.assignAlarmConfig(&config.alarmConfig2);
    break;
  default:
    return;
  }
}
