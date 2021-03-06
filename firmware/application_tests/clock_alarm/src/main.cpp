/*  Show UTC time
 *  Update UTC time once a day at 2AM.
 */
#include "Arduino.h"
#include "TimeSync.h"
#include "VisualElements.h"
#include "alarmcalendar.h"
#include "DisplayOnOff.h"
#include <DFMiniMp3.h>
#include "inputs.h"
#include "Mp3Notify.h"
#include "LedDriverDimming.h"
#include "pins.h"
#include <SparkFunSX1509.h> // Include SX1509 library

const int SECONDS_IN_30_MINS = 1800;
AlarmCalendar ac1(2);
HardwareSerial *ser1 = &Serial;
bool isMusicPlaying = false;
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial2);
LedDriverDimming ldd(pin_en_sun, pin_pwmh, pin_pwml);
SX1509 io1, io2;
const byte IO1_SX1509_ADDRESS = 0x3E; // SX1509 I2C address
const byte IO2_SX1509_ADDRESS = 0x3F; // SX1509 I2C address

//Variables to be stored in a user config
//Alarm time:
byte alarmHours = 21, alarmMinutes = 45;
WEEKDAYS wd = (WEEKDAYS)(WD_FRIDAY);
//Other settings
byte volume = 10;
byte track = 20;
float finalSunBrightness = 50; //percent.
bool isBrightnessSet = false;

void setup()
{
    while (!*ser1)
        ;
    ser1->begin(115200);
    ser1->printf("Build %s\r\n", __TIMESTAMP__); //only updated when this file is being recompiled.
    initClockSource();
    if (!io1.begin(IO1_SX1509_ADDRESS))
    {
        while (1)
            ;
    }
    if (!io2.begin(IO2_SX1509_ADDRESS))
    {
        while (1)
            ;
    }

    // Use the internal 2MHz oscillator.
    // Set LED clock to 500kHz (2MHz / (2^(3-1)):
    io1.clock(INTERNAL_CLOCK_2MHZ, 3);
    io2.clock(INTERNAL_CLOCK_2MHZ, 3);

    if (!setupInputs(&io1, &io2))
    {
        Serial.println("Can't setup all input devices.");
        while (true)
            ;
    }
    if (!initVisualElements(&io1, &io2))
    {
        Serial.println("Failed to initialize.");
        while (1)
            ; // If we fail to communicate, loop forever.
    }
    if (!initDisplayOnOffControl())
    {
        ser1->println("Failed to initialize.");
        while (1)
            ; // If we fail to communicate, loop forever.
    }

    ac1.setAlarmTime(alarmHours, alarmMinutes);
    ac1.setWeekdays((AlarmCalendar::WEEKDAYS)wd);
    mp3.begin();
    if (volume > 30)
    {
        volume = 30;
    }
    if (track < 1)
    {
        track = 1;
    }
    byte totalTrackCount = mp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
    if (track > totalTrackCount)
    {
        track = totalTrackCount;
    }
    ldd.init();
}

bool isAlarmBusy()
{
    time_t localTime;
    return getLocalTimeSeconds(localTime) && ac1.isUnacknowledgedAlarmOnGoing(localTime);
}

void loop()
{
    byte hours, minutes;
    time_t localTime, totalSecondsToNextEvent;
    bool buttonPressed = false;
    DISPLAY_STATE ds = getDisplayState(buttonPressed, isAlarmBusy());
    bool alarmAcked = false;
    KEY_CODE key;

    if (getLocalTimeSeconds(localTime))
    {
        if (isButtonChanged(key))
        {
            ac1.acknowledgeAlarm();
            alarmAcked = true;
        }
        //Redraw LED array
        if (ds == DISPLAY_TURNED_ON || isNewMinuteStarted(localTime, hours, minutes) || alarmAcked)
        {
            showClockTime(hours, minutes, isStillSynced(), ac1.isUnacknowledgedAlarmOnGoing(localTime), true);
            Chronos::DateTime::now().printTo(*ser1);
            Serial.println();
        }
        //Redraw alarm LCD
        if ((ds == DISPLAY_TURNED_ON) || (ds == DISPLAY_ON))
        {
            showAlarmDisplay(ac1.isAlarmIn24Hours(localTime) ? AL_BOTH_ON : AL_BOTH_OFF);
        }
        if (ac1.isAlarmIn24Hours(localTime))
        {
            showAlarmTime(alarmHours, alarmMinutes);
        }
        //Redraw Weekday LEDs
        showWeekDay(wd);
        //Handle music
        if (ac1.isUnacknowledgedAlarmOnGoing(localTime))
        {
            if (!isMusicPlaying)
            {
                mp3.setVolume(volume);
                mp3.loopGlobalTrack(track);
                isMusicPlaying = true;
            }
        }
        else
        {
            if (isMusicPlaying)
            {
                mp3.stop();
                isMusicPlaying = false;
            }
        }
        //Handle sun rise emulation
        if (ac1.isUnacknowledgedAlarmOnGoing(localTime))
        {
            if (!isBrightnessSet)
            {
                ldd.enable(true);
                ldd.setBrightness(finalSunBrightness);
                isBrightnessSet = true;
            }
        }
        else
        {
            if (ac1.getSecondsToStartOfNextEvent(localTime, totalSecondsToNextEvent) && totalSecondsToNextEvent < SECONDS_IN_30_MINS)
            {
                float brightness = finalSunBrightness * (SECONDS_IN_30_MINS - totalSecondsToNextEvent) / SECONDS_IN_30_MINS;
                ldd.enable(true);
                ldd.setBrightness(brightness);
            }
            else
            {
                ldd.enable(false);
                isBrightnessSet = false;
            }
        }
    }

    //Brightness control for all elements
    switch (ds)
    {
    case DISPLAY_TURNED_ON:
        setVisible(true);
        break;
    case DISPLAY_ON:
        setBrightness(getDisplayBrightness(isAlarmBusy()));
        break;
    case DISPLAY_TURNED_OFF:
        setVisible(false);
        break;
    case DISPLAY_OFF:
    default:
        break;
    }

    redraw();

    delay(100);
}