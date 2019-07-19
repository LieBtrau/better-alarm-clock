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

EepromConfig config;

extern ActionMgr actionMgr;

void setup()
{
  Serial.begin(115200);
  //config1.dayNight = 8;
  //Serial.println(EEPROM_writeAnything(0, config1), DEC);
  EepromConfig tempConfig;
  if (EEPROM_readAnything(0, tempConfig))
  {
    memcpy(&config, &tempConfig, sizeof(config));
  }
  assignCommonConfig(&config.commConfig);
  assignAlarmConfig(&config.alarmConfig1);
  actionMgr.assignCommonConfig(&config.commConfig);
  actionMgr.assignAlarmConfig(&config.alarmConfig1);
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
