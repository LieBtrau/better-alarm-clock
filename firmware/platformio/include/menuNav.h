#pragma once
#include "Adafruit_MCP23017.h"
#include "AlarmManager.h"
#include "ButtonManager.h"
#include "DisplayBrightness.h"
#include "KeyboardScan.h"
#include "Max72xxPanel.h"
#include "Menu.h"
#include "millisDelay.h"

void playSong(byte songnr);
void setVolume(byte volume);
void stopSong();
void showLightness(byte i);
void stopLightness();

class MenuMgr
{
public:
    typedef void (*voidFuncPtrVoid)(void);
    typedef enum
    {
        SHOW_SPLASH,
        SHOW_CLOCK,
        SETUP_ALARM1,
        SETUP_ALARM2
    } STATE;
    MenuMgr(Max72xxPanel *ledArray, DisplayBrightness *disp, Adafruit_MCP23017 *mcp, RotaryEncoderConsumer *prec, AlarmManager *palarms);
    void init(byte totalTrackCount);
    bool loop();
    void setSoonestAlarm(AlarmConfig *soonestAlarm);
    void setClockSynced(ClockTime time);
    void keyChanged(byte key);
    void setSaveConfigEvent(voidFuncPtrVoid evHandler);

private:
    STATE state = SHOW_SPLASH;
    void showLedState();
    void showAlarm(byte alarmNrnr);
    void showMenu();
    void showSyncAnimation();
    void rotaryEncoderAttachment(byte key);
    void assignAlarmConfig(AlarmManager *curAlarm);
    AlarmManager *alarms;
    AlarmConfig *nextAlarm = nullptr;
    voidFuncPtrVoid saveConfig = nullptr;
    bool settingAlarmHours = true;
    byte _brightness = 0xFF;
    bool displayWasOn = true;
    millisDelay _charliePlexingTimer;
    millisDelay _clockRefreshTimer;
    millisDelay _syncingTimer;
    millisDelay _setupTimeoutTimer;
    Max72xxPanel *matrix;
    RotaryEncoderConsumer *rec;
    ClockTime curTime;
    DisplayBrightness *dispbright;
    FieldParameter lightness = {0, nullptr, 100, 5, showLightness, stopLightness};
    FieldParameter volume = {0, nullptr, 30, 1, setVolume, stopSong};
    SelectParameter song = {nullptr, 1, 10, playSong, stopSong};
    FieldParameter hours = {0, nullptr, 23, 1, nullptr, nullptr};
    FieldParameter minutes = {0, nullptr, 55, 5, nullptr, nullptr};
    LedMatrixField fldLightness;
    LedMatrixField fldVolume;
    LedMatrixSelect sldSong;
    ClockFace clockface;
    Chaplex myCharlie;
    //Objects for controlling alarm settings : sun light emulation intensity, song choice, song volume
    bool matrixFields[3] = {false, false, false};
    LedToggle tglLightness = {&myCharlie, &ledMatrix[LIGHTNESS], matrixFields};
    LedToggle tglVolume = {&myCharlie, &ledMatrix[VOLUME], matrixFields + 1};
    LedToggle tglSongChoice = {&myCharlie, &ledMatrix[SONGCHOICE], matrixFields + 2};
    ParameterPushButton btnLightness = {LIGHTNESS, &tglLightness, &fldLightness};
    ParameterPushButton btnVolume = {VOLUME, &tglVolume, &fldVolume};
    ParameterPushButton btnSong = {SONGCHOICE, &tglSongChoice, &sldSong};
    ParameterButtonManager mgrBtnAlarm;
    //Extra alarm settings: enabling and disabling days of week.
    LedToggle tglMonday = {&myCharlie, &ledMatrix[MONDAY], nullptr};
    LedToggle tglTuesday = {&myCharlie, &ledMatrix[TUESDAY], nullptr};
    LedToggle tglWednesday = {&myCharlie, &ledMatrix[WEDNESDAY], nullptr};
    LedToggle tglThursday = {&myCharlie, &ledMatrix[THURSDAY], nullptr};
    LedToggle tglFriday = {&myCharlie, &ledMatrix[FRIDAY], nullptr};
    LedToggle tglSaturday = {&myCharlie, &ledMatrix[SATURDAY], nullptr};
    LedToggle tglSunday = {&myCharlie, &ledMatrix[SUNDAY], nullptr};
    TogglePushButton btnMonday = {MONDAY, &tglMonday};
    TogglePushButton btnTuesday = {TUESDAY, &tglTuesday};
    TogglePushButton btnWednesday = {WEDNESDAY, &tglWednesday};
    TogglePushButton btnThursday = {THURSDAY, &tglThursday};
    TogglePushButton btnFriday = {FRIDAY, &tglFriday};
    TogglePushButton btnSaturday = {SATURDAY, &tglSaturday};
    TogglePushButton btnSunday = {SUNDAY, &tglSunday};
    ToggleButtonManager mgrBtnWeekday;
    bool bAlarmSelected = false;
    LedToggle tglAlarm = {&myCharlie, &ledMatrix[ALARMTIME], &bAlarmSelected};
    bool bMenuSelected = false;
    LedToggle tglMenu = {&myCharlie, &ledMatrix[MENU], &bMenuSelected};
    Adafruit_7segment matrix7;
    SevenSegmentField fldHours = {&matrix7, SevenSegmentField::LEFTPOS, &hours};
    SevenSegmentField fldMinutes = {&matrix7, SevenSegmentField::RIGHTPOS, &minutes};
    ActionPushButton alarmTimeButton = {ALARMTIME, &tglAlarm};
    ActionPushButton menuButton = {MENU, &tglMenu};
    KeyboardScan keyb;
    CharlieLed ledMatrix[15] = {
        //{anode_registerbit, cathode_registerbit},
        {2, 1}, //D4 - ALARMTIME = 0
        {1, 2}, //D3 - SONGCHOICE = 1
        {1, 0}, //D2 - VOLUME = 2
        {0, 1}, //D1 - LIGHTNESS = 3
        {0, 4}, //D14 - DAYNIGHTLEVEL = 4
        {2, 0}, //D10 - NIGHTDISPLAYBRIGHTNESS = 5
        {0, 2}, //D7 - MENU = 6
        {4, 0}, //D15 - DAYDISPLAYBRIGHTNESS = 7
        {1, 3}, //D13 - MONDAY = 8
        {2, 3}, //D5 - TUESDAY = 9
        {3, 2}, //D8 - WEDNESDAY = 10
        {3, 4}, //D6 - THURSDAY = 11
        {4, 3}, //D9 - FRIDAY = 12
        {4, 2}, //D12 - SATURDAY = 13
        {2, 4}  //D11 - SUNDAY = 14
    };
};

void writePinModes(byte data);
void writePullups(byte data);
void keyReleased(byte key);
void writeGpio(byte data);
byte readGpio();