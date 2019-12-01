#include "Arduino.h"
#include "menuNav.h"
#include "at24c.h"
#include "AlarmManager.h"
#include "pins.h"
#include "DisplayBrightness.h"
#include "Tasks.h"
#include "SongPlayer.h"
#include "RotaryEncoder_MCP23017.h"
void saveConfig();

typedef struct
{
  AlarmConfig alarmConfig[MAX_ALARMS];
} EepromConfig;

static Adafruit_MCP23017 mcp;
static RotaryEncoder_MCP23017 rec(&mcp, pinIRQ);
static RotaryEncoderConsumer recons(&rec);
static DisplayBrightness dispbright(sdaPin, sclPin, pin_PIR);
static SongPlayer sPlayer(&Serial2, pinPlayBusy);
static EepromConfig config;
static AlarmManager alarms[2] =
    {
        AlarmManager(&sPlayer, 0, &config.alarmConfig[0]),
        AlarmManager(&sPlayer, 0, &config.alarmConfig[1])};
static DcfUtcClock dcfclock(pin_DCF, true);
static Task_Time timetask(&dcfclock);
static Task_Alarms alarmstask((AlarmManager **)&alarms);
static EepromConfig readConfig;
// 32x16 LED Matrix elements
static const int numberOfHorizontalDisplays = 4;
static const int numberOfVerticalDisplays = 2;
static Max72xxPanel matrix(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
static MenuMgr menuMgr(&matrix, &dispbright, &mcp, &recons, alarms);
static HardwareSerial *ser1 = &Serial;
static AT24C eeprom1(AT24C::AT24C02, 0x50, &Wire);
static const byte CFG_MEM_ADDRESS = 0;

void setup()
{
  while (!*ser1)
    ;
  ser1->begin(115200);
  Wire.begin();
  mcp.begin(); // use default address 0
  rec.init();
  recons.init();
  if (eeprom1.read(CFG_MEM_ADDRESS, readConfig))
  {
    memcpy(&config, &readConfig, sizeof(EepromConfig));
  }
  dcfclock.init();
  if (!sPlayer.init() || !dispbright.init())
  {
    while (true)
      ;
  }
  menuMgr.init(sPlayer.getTotalTrackCount());
  menuMgr.setSaveConfigEvent(saveConfig);
}

void loop()
{
  time_t localEpoch;

  bool keyPressed = menuMgr.loop();
  if (timetask.loop())
  {
    //Time is synced
    menuMgr.setClockSynced({true, timetask.getLocalHour(), timetask.getLocalMinute(), timetask.lastSyncOk()});
    localEpoch = timetask.getLocalEpoch();
    if (alarmstask.loop(localEpoch))
    {
      //alarm is ongoing
      if (keyPressed)
      {
        alarmstask.turnAlarmOff(localEpoch);
      }
      AlarmConfig *soonestAlarm = nullptr;
      if (alarmstask.getSoonestAlarm(localEpoch, soonestAlarm))
      {
        menuMgr.setSoonestAlarm(soonestAlarm);
      }
      else
      {
        menuMgr.setSoonestAlarm(nullptr);
      }
    }
  }
  else
  {
    menuMgr.setClockSynced({false, 0, 0, false});
  }

  delay(1); //sometimes MCP23017 stops responding after some time, adding this delay "fixes" it.
}

void saveConfig()
{
  if (memcmp(&config, &readConfig, sizeof(EepromConfig)))
  {
    if (!eeprom1.write(CFG_MEM_ADDRESS, config))
    {
      return;
    }
    memcpy(&readConfig, &config, sizeof(EepromConfig));
  }
}

// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t, const char *tz)
{
  char buf[32];
  char m[4]; // temporary storage for month string (DateStrings.cpp uses shared buffer)
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
          hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
  Serial.println(buf);
}
