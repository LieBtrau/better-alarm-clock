#include "Weekdays.h"
#include "AsyncDelay.h"
#include "inputs.h"

static WEEKDAYS* weekdays = nullptr;
static AsyncDelay weekdayBtnTimeout(500, AsyncDelay::MILLIS);


void setWeekdayPointer(WEEKDAYS* pwd)
{
    weekdays = pwd;
}

/**
 * \return true when a weekday changed state
 */
bool processButtonEvent(KEY_CODE key)
{
    static WEEKDAYS day = WD_NODAY;
    if (key != 0)
    {
        //key pressed, start timeout
        weekdayBtnTimeout.restart();
        switch (key)
        {
        case KEY_MONDAY:
            day = WD_MONDAY;
            break;
        case KEY_TUESDAY:
            day = WD_TUESDAY;
            break;
        case KEY_WEDNESDAY:
            day = WD_WEDNESDAY;
            break;
        case KEY_THURSDAY:
            day = WD_THURSDAY;
            break;
        case KEY_FRIDAY:
            day = WD_FRIDAY;
            break;
        case KEY_SATURDAY:
            day = WD_SATURDAY;
            break;
        case KEY_SUNDAY:
            day = WD_SUNDAY;
            break;
        default:
            return false;
        }
        return false;
    }
    else
    {
        if (!weekdayBtnTimeout.isExpired())
        {
            //Button released too soon
            return false;
        }
        *weekdays = (WEEKDAYS)(*weekdays ^ day);
    }
    return true;
}

void showWeek()
{
    Serial.println(*weekdays, HEX);
}