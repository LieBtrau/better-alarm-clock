
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Visuals.h"
#include "Max72xxPanel.h"
#include <SparkFunSX1509.h>
#include "pins.h"
#include "VisualElements.h"

const int numberOfHorizontalDisplays = 4;
const int numberOfVerticalDisplays = 2;
const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address

//Hardware peripherals
Max72xxPanel matrix = Max72xxPanel(pinMOSI, pinSCLK, pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
Adafruit_7segment sevenSegment = Adafruit_7segment();
SX1509 io1, io2; // Create an SX1509 object to be used throughout
//LedDriverDimming ldd(pin_en_sun, pin_pwmh, pin_pwml);

//Visual elements
//--Alarm LCD elements
SevenSegmentField alarmHoursDisplay(&sevenSegment, 0);
SevenSegmentField alarmMinutesDisplay(&sevenSegment, 3);
//--LED Array elements
ClockFace cf(&matrix);
LedMatrixField lmf_SunLightBrightness(&matrix, {0, 0}, {11, 2}, 10);
LedMatrixField lmf_Volume(&matrix, {0, 7}, {11, 9}, 10);
LedMatrixSelect lms_SongChoice(&matrix, {0, 13}, {11, 15}, 10);
//--LED elements
LedToggle ltSunLight(&io2, 4);
LedToggle ltVolume(&io2, 5);
LedToggle ltSongChoice(&io2, 6);
LedToggle ltAlarm(&io2, 7);
LedToggle ltMonday(&io1, 4);
LedToggle ltTuesday(&io1, 5);
LedToggle ltWednesday(&io1, 6);
LedToggle ltThursday(&io1, 7);
LedToggle ltFriday(&io1, 12);
LedToggle ltSaturday(&io1, 13);
LedToggle ltSunday(&io1, 14);
LedToggle *weekdays[7] = {&ltMonday, &ltTuesday, &ltWednesday, &ltThursday, &ltFriday, &ltSaturday, &ltSunday};
//--LED panel
//SunRiseEmulation sre(&ldd);

byte lastBrightNess = 0xFF;

bool initVisualElements()
{
    //Initialize hardware peripherals
    matrix.init();
    sevenSegment.begin(0x70);
    if (!io1.begin(IO1_SX1509_ADDRESS))
    {
        return false;
    }
    if (!io2.begin(IO2_SX1509_ADDRESS))
    {
        return false;
    }
    io1.clock(INTERNAL_CLOCK_2MHZ, 3);
    io2.clock(INTERNAL_CLOCK_2MHZ, 3);
    //ldd.init();

    //Initialize visual elements
    ltSunLight.init();
    ltVolume.init();
    ltSongChoice.init();
    ltAlarm.init();
    for (int i = 0; i < 7; i++)
    {
        weekdays[i]->init();
    }
    // lmf_SunLightBrightness.setVisible(true);
    // lms_SongChoice.setVisible(true);
    ltSunLight.setVisible(true);
    ltVolume.setVisible(true);
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
    if (cf.render() | lmf_SunLightBrightness.render() | lms_SongChoice.render())
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

void showAlarmDisplay(ALARM_DISPLAY ad)
{
    static ALARM_DISPLAY _ad = AL_BOTH_OFF;
    if (ad == _ad)
    {
        return;
    }
    _ad = ad;
    alarmHoursDisplay.setVisible((ad == AL_HOURS_ONLY) || (ad == AL_BOTH_ON) ? true : false);
    alarmMinutesDisplay.setVisible((ad == AL_MINUTES_ONLY) || (ad == AL_BOTH_ON) ? true : false);
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
    cf.setVisible(true);

    cf.setTime(hours, minutes, true);
    cf.setFlashMode(false);
    cf.setValidity(true);
    cf.setFlashMode(alarmOngoing);
}

void showSunlightSetting(byte value)
{
    cf.setVisible(false);
    lmf_SunLightBrightness.setVisible(true);
    lmf_Volume.setVisible(false);
    lms_SongChoice.setVisible(false);
}

void showSongVolume(byte value)
{
    cf.setVisible(false);
    lmf_SunLightBrightness.setVisible(false);
    lmf_Volume.setVisible(true);
    lms_SongChoice.setVisible(false);
}

void showSongChoice(byte value)
{
    cf.setVisible(false);
    lmf_SunLightBrightness.setVisible(false);
    lmf_Volume.setVisible(false);
    lms_SongChoice.setVisible(true);
}
