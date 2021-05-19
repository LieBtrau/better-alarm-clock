
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Visuals.h"
#include "Max72xxPanel.h"
#include <SparkFunSX1509.h>
#include "pins.h"
#include "VisualElements.h"

const int numberOfHorizontalDisplays = 4;
const int numberOfVerticalDisplays = 2;

//Hardware peripherals
Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
Adafruit_7segment sevenSegment = Adafruit_7segment();
//LedDriverDimming ldd(pin_en_sun, pin_pwmh, pin_pwml);

//Visual elements
//--Alarm LCD elements
SevenSegmentField alarmHoursDisplay(&sevenSegment, 0);
SevenSegmentField alarmMinutesDisplay(&sevenSegment, 3);
//--LED Array elements
ClockFace cf(&matrix);
LedMatrixField lmf_SunLightBrightness(&matrix, {20, 0}, {31, 2}, 10);
LedMatrixField lmf_Volume(&matrix, {20, 7}, {31, 9}, 10);
LedMatrixSelect lms_SongChoice(&matrix, {20, 13}, {31, 15}, 10);
//--LED elements
LedToggle ltSunLight(pin_sunlight);
LedToggle ltVolume(pin_volume);
LedToggle ltSongChoice(pin_songChoice);
LedToggle ltAlarm(pin_alarm);
LedToggle ltMonday(pin_Monday);
LedToggle ltTuesday(pin_Tuesday);
LedToggle ltWednesday(pin_Wednesday);
LedToggle ltThursday(pin_Thursday);
LedToggle ltFriday(pin_Friday);
LedToggle ltSaturday(pin_Saturday);
LedToggle ltSunday(pin_Sunday);
LedToggle *weekdays[7] = {&ltMonday, &ltTuesday, &ltWednesday, &ltThursday, &ltFriday, &ltSaturday, &ltSunday};
//--LED panel
//SunRiseEmulation sre(&ldd);

byte lastBrightNess = 0xFF;

bool initVisualElements(SX1509 *io1, SX1509 *io2) // Create an SX1509 object to be used throughout
{
    //Initialize hardware peripherals
    matrix.init();
    sevenSegment.begin(0x70);
    //ldd.init();

    //Initialize visual elements
    ltSunLight.init(io2);
    ltVolume.init(io2);
    ltSongChoice.init(io2);
    ltAlarm.init(io2);
    for (int i = 0; i < 7; i++)
    {
        weekdays[i]->init(io1);
    }
    // sre.setVisible(true);
    cf.setVisible(true);
    //cf.setFlashMode(true);  //It's better to disable flash mode during syncing to avoid EM-interference.
    return true;
}

// \param brightness 0-15
void setBrightness(byte brightness)
{
    if (brightness == lastBrightNess || brightness > 15)
    {
        return;
    }
    lastBrightNess = brightness;

    sevenSegment.setBrightness(brightness);
    matrix.setIntensity(brightness);
    byte ledToggleBrightness = 15 + (brightness << 4);
    ltSunLight.setBrightness(ledToggleBrightness);
    ltVolume.setBrightness(ledToggleBrightness);
    ltSongChoice.setBrightness(ledToggleBrightness);
    ltAlarm.setBrightness(ledToggleBrightness);
    for (int i = 0; i < 7; i++)
    {
        weekdays[i]->setBrightness(ledToggleBrightness);
    }
}

void setVisible(bool isVisible)
{
    if (!isVisible)
    {
        cf.setVisible(false);
        alarmHoursDisplay.setVisible(false);
        alarmMinutesDisplay.setVisible(false);
        for (int i = 0; i < 7; i++)
        {
            weekdays[i]->setVisible(false);
        }
        lmf_SunLightBrightness.setVisible(false);
        lmf_Volume.setVisible(false);
        lms_SongChoice.setVisible(false);
    }

    ltSunLight.setVisible(isVisible);
    ltVolume.setVisible(isVisible);
    ltSongChoice.setVisible(isVisible);
    ltAlarm.setVisible(isVisible);
}

void redraw()
{
    if (lmf_SunLightBrightness.render() | lmf_Volume.render() | lms_SongChoice.render() | cf.render())
    {
        //true must be passed in, otherwise flashing doesn't work.
        matrix.write(true);
    }
    if (alarmMinutesDisplay.render() | alarmHoursDisplay.render()) //don't use double |, because both statements need to be executed.
    {
        sevenSegment.writeDisplay();
    }
    for (int i = 0; i < 7; i++)
    {
        weekdays[i]->render();
    }
    ltSunLight.render();
    ltVolume.render();
    ltSongChoice.render();
    ltAlarm.render();
    //sre.render();
}

void showWeekDay(WEEKDAYS wd)
{
    for (int i = 0; i < 7; i++)
    {
        weekdays[i]->setVisible(wd & (1 << i) ? true : false);
    }
}

void showAlarmDisplay(ALARM_DISPLAY ad, bool setup)
{
    if (setup)
    {
        lmf_SunLightBrightness.setVisible(false);
        lmf_Volume.setVisible(false);
        lms_SongChoice.setVisible(false);
        cf.setVisible(false);
        ltAlarm.setVisible(true);
        redraw();
    }

    alarmHoursDisplay.setVisible((ad == AL_HOURS_ONLY) || (ad == AL_BOTH_ON) ? true : false);
    alarmMinutesDisplay.setVisible((ad == AL_MINUTES_ONLY) || (ad == AL_BOTH_ON) ? true : false);

    ltSunLight.setVisible(false);
    ltVolume.setVisible(false);
    ltSongChoice.setVisible(false);
}

void showAlarmTime(byte hours, byte minutes)
{
    alarmHoursDisplay.setValue(hours);
    alarmMinutesDisplay.setValue(minutes);
}

void showClockTime(byte hours, byte minutes, bool synced, bool alarmOngoing)
{
    lmf_SunLightBrightness.setVisible(false);
    lmf_Volume.setVisible(false);
    lms_SongChoice.setVisible(false);
    redraw();
    cf.setVisible(true);

    cf.setTime(hours, minutes, true);
    cf.setFlashMode(false);
    cf.setValidity(true);
    cf.setFlashMode(alarmOngoing);

    ltSunLight.setVisible(false);
    ltVolume.setVisible(false);
    ltSongChoice.setVisible(false);
    ltAlarm.setVisible(false);
}

void showSunlightSetting(byte value)
{
    cf.setVisible(false);
    lmf_Volume.setVisible(false);
    lms_SongChoice.setVisible(false);
    redraw();
    lmf_SunLightBrightness.setVisible(true);

    lmf_SunLightBrightness.setValue(value);

    ltSunLight.setVisible(true);
    ltVolume.setVisible(false);
    ltSongChoice.setVisible(false);
    ltAlarm.setVisible(false);
}

void showSongVolume(byte value)
{
    cf.setVisible(false);
    lmf_SunLightBrightness.setVisible(false);
    lms_SongChoice.setVisible(false);
    redraw();
    lmf_Volume.setVisible(true);

    lmf_Volume.setValue(value);

    ltSunLight.setVisible(false);
    ltVolume.setVisible(true);
    ltSongChoice.setVisible(false);
    ltAlarm.setVisible(false);
}

void showSongChoice(byte value)
{
    cf.setVisible(false);
    lmf_SunLightBrightness.setVisible(false);
    lmf_Volume.setVisible(false);
    redraw();
    lms_SongChoice.setVisible(true);

    lms_SongChoice.setValue(value);

    ltSunLight.setVisible(false);
    ltVolume.setVisible(false);
    ltSongChoice.setVisible(true);
    ltAlarm.setVisible(false);
}
