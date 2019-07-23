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
static EepromConfig readConfig;
extern ActionMgr actionMgr;
extern MenuMgr menuMgr;

void setup()
{
  Serial.begin(115200);

  if (EEPROM_readAnything(0, readConfig))
  {
    Serial.println("Using config from EEPROM.");
    memcpy(&config, &readConfig, sizeof(config));
  }
  menuMgr.assignCommonConfig(&config.commConfig);
  actionMgr.assignCommonConfig(&config.commConfig);
  if (!actionMgr.initPeripherals())
  {
    Serial.println("Can't init peripherals");
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

void saveConfig()
{
  if (memcmp(&config, &readConfig, sizeof(config)))
  {
    //only write to EEPROM if current config is different from the one read from the EEPROM
    EEPROM_writeAnything(0, config);
  }
}