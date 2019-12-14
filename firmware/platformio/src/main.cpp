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
void printDateTime(time_t t, const char *tz);

typedef struct
{
  AlarmConfig alarmConfig[MAX_ALARMS];
} EepromConfig;

static Adafruit_MCP23017 mcp;
static RotaryEncoder_MCP23017 rec(&mcp, pinIRQ);
static RotaryEncoderConsumer recons(&rec);
static DisplayBrightness dispbright(pin_PIR);
static SongPlayer sPlayer(&Serial2, pinPlayBusy);
static EepromConfig config;
static AlarmManager alarms[2] =
    {
        AlarmManager(&sPlayer, 0),
        AlarmManager(&sPlayer, 0)};

static DcfUtcClock dcfclock(pin_DCF, true);
static Task_Time timetask(&dcfclock);
static Task_Alarms alarmstask(alarms);
static EepromConfig readConfig;
// 32x16 LED Matrix elements
static const int numberOfHorizontalDisplays = 4;
static const int numberOfVerticalDisplays = 2;
static Max72xxPanel matrix(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
static MenuMgr menuMgr(&matrix, &dispbright, &mcp, &recons, alarms);
static HardwareSerial *ser1 = &Serial;
static AT24C eeprom1(AT24C::AT24C02, 0x50, &Wire);
static const byte CFG_MEM_ADDRESS = 0;
static millisDelay updateClockDelay;

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
  updateClockDelay.start(2000);
  dcfclock.init();
  if (!sPlayer.init() || !dispbright.init())
  {
    while (true)
      ;
  }
  menuMgr.init(sPlayer.getTotalTrackCount());
  menuMgr.setSaveConfigEvent(saveConfig);
  for (byte i = 0; i < MAX_ALARMS; i++)
  {
    alarms[i].setConfig(&config.alarmConfig[i]);
  }
}

void loop()
{
  time_t localEpoch;
  bool keyReleased = menuMgr.loop();
  
  if (timetask.loop())
  {
    //Time is synced
    localEpoch = timetask.getLocalEpoch();
    if (updateClockDelay.justFinished())
    {
      updateClockDelay.restart();
      menuMgr.setClockSynced({timetask.getLocalHour(), timetask.getLocalMinute(), timetask.lastSyncOk(), true});
    }
    if (alarmstask.loop(localEpoch) && keyReleased)
    {
      //alarm is ongoing
      alarmstask.turnAlarmOff(localEpoch);
    }
    byte soonestAlarmIndex = 0xFF;
    if (alarmstask.getSoonestAlarm(localEpoch, soonestAlarmIndex))
    {
      menuMgr.setSoonestAlarm(alarms[soonestAlarmIndex].getConfig());
    }
    else
    {
      menuMgr.setSoonestAlarm(nullptr);
    }
  }
  else
  {
    alarmstask.loop(0); //run alarm task even though time is not set yet.  It's needed to setup up the music of the alarm.
    menuMgr.setClockSynced({0, 0, false, false});
  }
  delay(1); //stop locking up the MCP23017
}

void saveConfig()
{
  for (byte i = 0; i < MAX_ALARMS; i++)
  {
    alarms[i].setConfig(&config.alarmConfig[i]);
  }
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
  static long timediff = 0;
  long diff = t - millis() / 1000;
  if (diff != timediff && abs(timediff - diff) > 1)
  {
    char buf[32];
    char m[4]; // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
            hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
    Serial.print(buf);
    Serial.print(" ");
    Serial.print(timediff - diff);
    Serial.print(" ");
    Serial.println(millis());
    timediff = diff;
  }
}