#include "Tasks.h"
#include "menuNav.h"
#include "rotaryEncoder.h"
#include "Fonts/TomThumb.h"

static Adafruit_MCP23017 *mcp = nullptr;
static AlarmManager *currentAlarm = nullptr;

MenuMgr::MenuMgr(Max72xxPanel *ledArray, DisplayBrightness *disp, Adafruit_MCP23017 *pmcp,
                 RotaryEncoderConsumer *prec, AlarmManager *palarms) : alarms(palarms),
                                                                       matrix(ledArray),
                                                                       dispbright(disp),
                                                                       fldLightness(matrix, {0, 0}, {11, 2}, &lightness),
                                                                       fldVolume(matrix, {0, 7}, {11, 9}, &volume),
                                                                       sldSong(matrix, {0, 13}, {11, 15}, &song),
                                                                       clockface(matrix)
{
  mcp = pmcp;
  rec = prec;
  mgrBtnAlarm.addButton(&btnLightness);
  mgrBtnAlarm.addButton(&btnVolume);
  mgrBtnAlarm.addButton(&btnSong);
  mgrBtnAlarm.attachRotaryEncoder(rec);

  mgrBtnWeekday.addButton(&btnMonday);
  mgrBtnWeekday.addButton(&btnTuesday);
  mgrBtnWeekday.addButton(&btnWednesday);
  mgrBtnWeekday.addButton(&btnThursday);
  mgrBtnWeekday.addButton(&btnFriday);
  mgrBtnWeekday.addButton(&btnSaturday);
  mgrBtnWeekday.addButton(&btnSunday);

  //linking the parameters prevents stopping the current action when jumping between parameters during setup.
  //In this case, when a song has been chosen, it will be playing.  Upon selecting the volume parameter, the song must continue and not start all over again.
  sldSong.setLinkedParameter(&fldVolume);
  fldVolume.setLinkedParameter(&sldSong);

  _clockRefreshTimer.start(500);
}

void MenuMgr::showMenu()
{
  rec->setConsumer(nullptr, false);
  if (curTime.valid)
  {
    clockface.setVisible(true);
  }
  mgrBtnAlarm.disable();
  mgrBtnWeekday.disable();
  matrix->fillScreen(0);
  if (saveConfig != nullptr)
  {
    saveConfig();
  }
  state = SHOW_CLOCK;
}

/**
 * \return true when a key has changed state (pressed or released)
 */
bool MenuMgr::loop()
{
  keyb.updateKeys(writeGpio, readGpio);
  byte lastKey;
  bool keyReleased = keyb.getLastKeyReleased(&lastKey);
  byte brightness;
  bool displayOn = dispbright->isDisplayOn(keyReleased);
  if (displayOn)
  {
    if (rec->render() | fldHours.render(!displayWasOn) | fldMinutes.render(!displayWasOn) | !displayWasOn)
    {
      matrix7.writeDisplay();
    }
    if (dispbright->getDisplayBrightness(brightness) && brightness != _brightness)
    {
      _brightness = brightness;
      matrix7.setBrightness(brightness); // 0 -> 15
      matrix->setIntensity(brightness);  // 0 -> 15
    }
    matrix->write(); // Send bitmap to display
    switch (state)
    {
    case SHOW_SPLASH:
      // matrix.setFont(&Picopixel);
      // matrix.setCursor(4, 10);
      matrix->setFont(&TomThumb);
      matrix->setCursor(4, 10);
      matrix->print("SPLASH");
      delay(1000);
      matrix->fillScreen(0);
      state = SHOW_CLOCK;
      break;
    case SHOW_CLOCK:
      showLedState();
      //show current time
      if (keyReleased && lastKey == MENU)
      {
        assignAlarmConfig(&alarms[ALARM1]);
        clockface.setVisible(false);
        fldHours.setVisible(true);
        fldMinutes.setVisible(true);
        mgrBtnAlarm.enable();
        mgrBtnWeekday.enable();
        showAlarm(1);
        state = SETUP_ALARM1;
        _setupTimeoutTimer.start(120000);
      }
      else
      {
        if (curTime.valid)
        {
          if (_clockRefreshTimer.justFinished())
          {
            _clockRefreshTimer.restart();
            clockface.setVisible(true);
            clockface.setTime(curTime);
            clockface.render();
          }
        }
        else
        {
          showSyncAnimation();
        }
        //show next alarm
        if (nextAlarm != nullptr)
        {
          hours.cur = &nextAlarm->time.hour;
          minutes.cur = &nextAlarm->time.mins;
          fldHours.setVisible(true);
          fldMinutes.setVisible(true);
        }
        else
        {
          fldHours.setVisible(false);
          fldMinutes.setVisible(false);
        }
      }
      break;
    case SETUP_ALARM1:
      showLedState();
      mgrBtnAlarm.render(!displayWasOn);
      mgrBtnWeekday.render(!displayWasOn);
      alarmTimeButton.render(!displayWasOn);
      rec->poll();
      if (_setupTimeoutTimer.justFinished())
      {
        showMenu();
      }
      if (keyReleased)
      {
        rotaryEncoderAttachment(lastKey);
        if (lastKey == MENU)
        {
          _setupTimeoutTimer.restart();
          assignAlarmConfig(&alarms[ALARM2]);
          showAlarm(2);
          mgrBtnAlarm.render(true);
          mgrBtnWeekday.render(true);
          alarmTimeButton.render(true);
          fldHours.render(true);
          fldMinutes.render(true);
          matrix7.writeDisplay();
          state = SETUP_ALARM2;
        }
      }
      break;
    case SETUP_ALARM2:
      showLedState();
      mgrBtnAlarm.render(!displayWasOn);
      mgrBtnWeekday.render(!displayWasOn);
      alarmTimeButton.render(!displayWasOn);
      rec->poll();
      if (keyReleased)
      {
        rotaryEncoderAttachment(lastKey);
        if (lastKey == MENU)
        {
          showMenu();
        }
      }
      if (_setupTimeoutTimer.justFinished())
      {
        showMenu();
      }
      break;
    default:
      break;
    }
  }
  else
  {
    dispbright->getDisplayBrightness(brightness);
    if (displayWasOn)
    {
      matrix->fillScreen(0);
      matrix->write(true);
      matrix7.clear();
      matrix7.writeDisplay();
      byte pinModes = mcp->readPinMode(1);
      mcp->writePinMode(1, pinModes | 0x1F); //set all LED pins as input
    }
  }
  displayWasOn = displayOn;
  return keyReleased;
}

void MenuMgr::rotaryEncoderAttachment(byte key)
{
  bool encoderCoupled = false;
  if (mgrBtnAlarm.keyPressed(key) || mgrBtnWeekday.keyPressed(key))
  {
    encoderCoupled = true;
  }
  if (key == ALARMTIME)
  {
    rec->setConsumer(settingAlarmHours ? &fldHours : &fldMinutes, true);
    settingAlarmHours = !settingAlarmHours;
    encoderCoupled = true;
  }
  if (!encoderCoupled)
  {
    rec->setConsumer(nullptr, false);
  }
}

void MenuMgr::assignAlarmConfig(AlarmManager *curAlarm)
{
  currentAlarm = curAlarm;
  AlarmConfig *config = curAlarm->getConfig();
  lightness.cur = &config->lightness;
  volume.cur = &config->volume;
  song.cur = &config->song;
  hours.cur = &config->time.hour;
  minutes.cur = &config->time.mins;
  tglMonday.setSource(&config->time.weekdays[AlarmCalendar::dayToIndex(Chronos::Weekday::Monday)]);
  tglTuesday.setSource(&config->time.weekdays[AlarmCalendar::dayToIndex(Chronos::Weekday::Tuesday)]);
  tglWednesday.setSource(&config->time.weekdays[AlarmCalendar::dayToIndex(Chronos::Weekday::Wednesday)]);
  tglThursday.setSource(&config->time.weekdays[AlarmCalendar::dayToIndex(Chronos::Weekday::Thursday)]);
  tglFriday.setSource(&config->time.weekdays[AlarmCalendar::dayToIndex(Chronos::Weekday::Friday)]);
  tglSaturday.setSource(&config->time.weekdays[AlarmCalendar::dayToIndex(Chronos::Weekday::Saturday)]);
  tglSunday.setSource(&config->time.weekdays[AlarmCalendar::dayToIndex(Chronos::Weekday::Sunday)]);
}

void MenuMgr::init(byte totalTrackCount)
{
  song.max = totalTrackCount;
  matrix->init();
  matrix->fillScreen(0);
  matrix7.begin(0x70);
  matrix7.clear();
  keyb.init(writePinModes, writePullups);
  _charliePlexingTimer.start(5);
  _syncingTimer.start(500);
}

void MenuMgr::showLedState()
{
  if (_charliePlexingTimer.justFinished())
  {
    _charliePlexingTimer.restart();
    byte pinModes = mcp->readPinMode(1);
    byte gpioStates = mcp->readGPIO(1);
    if (myCharlie.showLedState(pinModes, gpioStates))
    {
      mcp->writePinMode(1, pinModes | 0x1F); //set all LED pins as input to avoid that unwanted LEDs light.
      mcp->writeGPIO(1, gpioStates);
      //unwanted LEDs would go on here, in between the two calls to the MCP23017
      mcp->writePinMode(1, pinModes);
    }
  }
}

void MenuMgr::setSoonestAlarm(AlarmConfig *soonestAlarm)
{
  nextAlarm = soonestAlarm;
}

void MenuMgr::setClockSynced(ClockTime time)
{
  curTime = time;
}

void MenuMgr::showSyncAnimation()
{
  static byte i = 0, j = 0;
  //should show some animation here, indicating clock is syncing time
  if (_syncingTimer.justFinished())
  {
    _syncingTimer.restart();
    matrix->writePixel(i, j, 1);
    if (i == 31)
    {
      j = j == 15 ? 0 : j + 1;
    }
    i = i == 31 ? 0 : i + 1;
  }
}

void MenuMgr::showAlarm(byte alarmNr)
{
  matrix->fillScreen(0);
  matrix->setFont(&TomThumb);
  matrix->setCursor(4, 10);
  matrix->print("ALARM");
  matrix->print(alarmNr);
  matrix->write(); // Send bitmap to display
  delay(500);
  matrix->fillScreen(0);
  matrix->setCursor(14, 10);
  matrix->print(alarmNr);
}

void MenuMgr::setSaveConfigEvent(voidFuncPtrVoid evHandler)
{
  saveConfig = evHandler;
}

void playSong(byte songnr)
{
  currentAlarm->playSong(songnr);
}

void setVolume(byte volume)
{
  currentAlarm->setVolume(volume);
}

void stopSong()
{
  currentAlarm->stopSong();
}

void showLightness(byte i)
{
  currentAlarm->showLightness(i);
}

void stopLightness()
{
  currentAlarm->stopLightness();
}

void writePinModes(byte data)
{
  mcp->writePinMode(0, data);
}

void writePullups(byte data)
{
  mcp->writePullUps(0, data);
}

void writeGpio(byte data)
{
  mcp->writeGPIO(0, data);
}

byte readGpio()
{
  return mcp->readGPIO(0);
}