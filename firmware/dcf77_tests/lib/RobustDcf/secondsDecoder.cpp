#include "secondsDecoder.h"

SecondsDecoder::SecondsDecoder()
{
    memset(_bins, 0, sizeof(_bins));
}

void SecondsDecoder::updateSeconds(const bool isSyncMark, const bool isLongPulse)
{
    /* Add two 60bit registers: one register for sync mark bit, one register for data bit.
     * Each time tick data comes in, it gets shifted into the two registers.
     * After that, a correlator is run on each register.  The results are added and stored in the current bin.
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

    //Detect 0-bit on second 0
    bounded_increment(_bins[_activeBin], _bitShifter & 1 ? -1 : 1);
    //Detect 1-bit on second 20
    bounded_increment(_bins[_activeBin], _bitShifter & 0x100000U ? 1 : -1);
    //Detect even parity over bits 21-28
    uint32_t parityCheck = _bitShifter & 0x1FE00000;
    bounded_increment(_bins[_activeBin], dataValid(parityCheck) ? 1 : -1);
    //Compiler bug : & bit operations on uint64 don't work.  Only the lower 32bits are taken into account.
    //Detect even parity over bits 29–35
    parityCheck = (_bitShifter>>4) & 0xFE000000;
    bounded_increment(_bins[_activeBin], dataValid(parityCheck) & 1 ? 1 : -1);
    //Detect even parity over bits 36–58
    parityCheck = (_bitShifter>>28) & 0x7fffff00;
    bounded_increment(_bins[_activeBin], dataValid(parityCheck) & 1 ? 1 : -1);
    //Detect sync mark on second 59
    bounded_increment(_bins[_activeBin], (isSyncMark && (!isLongPulse))? 6 : -6);

    //Find bin where correlation is maximum
    int8_t maxCorrelation = 0;
    _minuteStartBin = 0xFF;
    for (uint16_t bin = 0; bin < sizeof(_bins); ++bin)
    {
        if (_bins[bin] >= max(maxCorrelation, LOCK_THRESHOLD))
        {
            maxCorrelation = _bins[bin];
            _minuteStartBin = bin;
        }
    }
 
    //Advance current bin
    _activeBin = _activeBin < (SECONDS_PER_MINUTE - 1) ? _activeBin + 1 : 0;
}

//return false when second doesn't contain valid data
bool SecondsDecoder::getSecond(uint8_t& second)
{
    // we have to subtract 2 seconds
    //   1 because the seconds already advanced by 1 tick
    //   1 because the sync mark is not second 0 but second 59
    second = ((SECONDS_PER_MINUTE<<1) + _activeBin - 2 - _minuteStartBin);
    second %= SECONDS_PER_MINUTE;
    return _minuteStartBin != 0xFF;
}

void SecondsDecoder::bounded_increment(int8_t &value, int8_t N)
{
    if (value > 0)
    {
        value = (value > 128 - N ? 128 : value + N);
    }
    else
    {
        value = (value < -127 - N ? -127 : value + N);
    }
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
