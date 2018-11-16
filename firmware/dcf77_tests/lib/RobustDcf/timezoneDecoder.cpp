#include "timezoneDecoder.h"

TimeZoneDecoder::TimeZoneDecoder() : _bin(NR_OR_TIMEZONES){};

bool TimeZoneDecoder::update(SecondsDecoder::BITDATA *data)
{
    if (data->validBitCtr < SecondsDecoder::SECONDS_PER_MINUTE - STARTBIT)
    {
        //not enough valid samples in the data buffer
        return false;
    }
    int8_t score = 0;
    int8_t points = _isPredictionCEST ? 1 : -1;
    score = (data->bitShifter & CEST_BIT) ? points : -points;
    _bin.add(0, score);
    score += (data->bitShifter & CET_BIT) ? points : -points;
    _bin.add(1, score);
    return true;
}

void TimeZoneDecoder::setPrediction(bool isSummerTime)
{
    _isPredictionCEST = isSummerTime;
}

bool TimeZoneDecoder::getSecondsOffset(int16_t &offset)
{
    uint8_t bin = _bin.maximum(THRESHOLD);
    if(bin==0xFF)
    {
        return false;
    }
    //At startup _isPredictionCEST is false.
    //If we startup in winter, then bin 0 will be max and will remain max during summer time
    //If we startup in summer, then bin 1 will be max and will remain max during winter time
    //Sumer time (=two hour offset) happens :
    //  when bin 1 is max with _ispredictionCEST set false or when bin 0 is max with _ispredictionCEST set true.
    offset =  bin ^ (_isPredictionCEST ? 1 : 0) ? TWO_HOURS : ONE_HOUR;
    return true;
}
