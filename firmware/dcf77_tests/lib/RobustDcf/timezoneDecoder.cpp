#include "timezoneDecoder.h"

TimeZoneDecoder::TimeZoneDecoder(){};

bool TimeZoneDecoder::update(SecondsDecoder::BITDATA *data)
{
    if (data->validBitCtr < SecondsDecoder::SECONDS_PER_MINUTE - STARTBIT)
    {
        //not enough valid samples in the data buffer
        return false;
    }
    if ((data->bitShifter & CEST_BIT) && (_isSummerTime < INT8_MAX))
    {
        _isSummerTime++;
    }
    if ((data->bitShifter & CET_BIT) && (_isSummerTime > INT8_MIN))
    {
        _isSummerTime--;
    }
    if (data->bitShifter & TIMEZONE_CHANGE_BIT)
    {
        if (_timeZoneChangeAnnounced < UINT8_MAX)
        {
            _timeZoneChangeAnnounced++;
        }
    }
    else
    {
        if (_timeZoneChangeAnnounced > 0)
        {
            _timeZoneChangeAnnounced--;
        }
    }
    return true;
}

bool TimeZoneDecoder::getSecondsOffset(int16_t &offset, uint8_t &hour, uint8_t minute)
{
    if ((!minute) && _timeZoneChangeAnnounced > 0)
    {
        switch (hour)
        {
        case 3:
            if ((_isSummerTime > 0))
            {
                //First minute of winter time
                hour--; //Correct hour reading: should have been read as 2, but due to binning will be read as 3.
                _isSummerTime = -1;
            }
            break;
        case 2:
            if ((_isSummerTime < 0))
            {
                //First minute of summer time
                hour++; //Correct hour reading: should have been read as 3, but due to binning will be read as 2.
                _isSummerTime = 1;
            }
            break;
        default:
            break;
        }
    }
    offset = _isSummerTime > 0 ? TWO_HOURS : ONE_HOUR;
    return true;
}
