#include "secondsDecoder.h"

SecondsDecoder::SecondsDecoder():_bin(SECONDS_PER_MINUTE){}

void SecondsDecoder::updateSeconds(const bool isSyncMark, const bool isLongPulse)
{
    /* Each second, pulse data comes in.  It gets shifted into the bit shifter.
     * After that, a correlator is run on the bit shifter.  The results are added to the current bin.
     * The bin that has the highest score is the most likely to be the minute start.
     * Following markers will be used:
     *  - 0-bit on second 0
     *  - 1-bit on second 20
     *  - even parity over bits 21-28
     *  - even parity over bits 29–35
     *  - even parity over date bits 36–58
     *  - sync mark on second 59
     */

    //Shift in new data from right to left (because LSb is sent first)
    _bitShifter >>= 1;
    _bitShifter |= isLongPulse ? 0x800000000000000U : 0;
    int8_t score = 0;
    //Detect 0-bit on second 0
    score += _bitShifter & 1 ? -1 : 1;
    //Detect 1-bit on second 20
    score += _bitShifter & 0x100000U ? 1 : -1;
    //Detect even parity over bits 21-28
    uint32_t parityCheck = _bitShifter & 0x1FE00000;
    score += dataValid(parityCheck) ? 1 : -1;
    //Compiler bug : & bit operations on uint64 don't work.  Only the lower 32bits are taken into account.
    //Detect even parity over bits 29–35
    parityCheck = (_bitShifter >> 4) & 0xFE000000;
    score += dataValid(parityCheck) & 1 ? 1 : -1;
    //Detect even parity over bits 36–58
    parityCheck = (_bitShifter >> 28) & 0x7fffff00;
    score += dataValid(parityCheck) & 1 ? 1 : -1;
    //Detect sync mark on second 59
    score += (isSyncMark && (!isLongPulse)) ? 6 : -6;

    _bin.add(_activeBin, score);
    _minuteStartBin = _bin.maximum(LOCK_THRESHOLD);

    //Advance current bin
    _activeBin = _activeBin < (SECONDS_PER_MINUTE - 1) ? _activeBin + 1 : 0;

    uint8_t second = 0;
    if (getSecond(second) && second == 59)
    {
        _prevBitShifter = _bitShifter;
    }
}

//return false when second doesn't contain valid data
bool SecondsDecoder::getSecond(uint8_t &second)
{
    // we have to subtract 2 seconds
    //   1 because the seconds already advanced by 1 tick
    //   1 because the sync mark is not second 0 but second 59
    if (_minuteStartBin == 0xFF)
    {
        second = 0;
        return false;
    }
    second = ((SECONDS_PER_MINUTE << 1) + _activeBin - 2 - _minuteStartBin);
    second %= SECONDS_PER_MINUTE;
    return true;
}

bool SecondsDecoder::getTimeData(uint64_t &data)
{
    data = _prevBitShifter;
    return _minuteStartBin != 0xFF;
}

//Check if data not zero and if parity is even
bool SecondsDecoder::dataValid(uint64_t x)
{
    if (!x)
    {
        return false;
    }
    //https://stackoverflow.com/questions/9133279/bitparity-finding-odd-number-of-bits-in-an-integer#9133404
    x ^= x >> 32;
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x &= 0xf;
    return (0x6996 >> x) & 1 ? false : true;
}
