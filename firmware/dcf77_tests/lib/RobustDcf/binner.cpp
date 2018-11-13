#include "binner.h"

Binner::Binner(uint8_t startBit, uint8_t bitWidth, bool withParity, uint8_t lowestValue, uint8_t highestValue, int8_t lockThreshold) : _startBit(startBit), _bitWidth(bitWidth), _withParity(withParity), _lowestValue(lowestValue), _highestValue(highestValue), _lockThreshold(lockThreshold), _bin(highestValue - lowestValue + 1)
{
}

bool Binner::getTime(uint8_t &value)
{
    uint8_t bin = _bin.maximum(_lockThreshold);
    if(bin==0xFF)
    {
        return false;
    }
    //Convert index of the bin to a decimal value within the expected range.
    value = getValueInRange(bin);
    return true;
}

void Binner::advanceTick()
{
    _currentTick = _currentTick < _bin.size() - 1 ? _currentTick + 1 : 0;
}

/* Check validity of the data by correlating these with a calculated prediction value for each bin.
 * The matching score of the correlation gets added to the corresponding bin.
 */ 
void Binner::update(SecondsDecoder::BITDATA* data)
{
    if(data->validBitCtr<SecondsDecoder::SECONDS_PER_MINUTE-_startBit)
    {
        //not enough valid samples in the data buffer
        return;
    }
    uint64_t newData = data->bitShifter >> _startBit;
    newData &= (1 << (_bitWidth + (_withParity ? 1 : 0))) - 1;
    for (uint8_t i = 0; i < _bin.size(); i++)
    {
        uint8_t prediction = int2bcd(getValueInRange(i));
        if (_withParity && parityOdd(prediction))
        {
            prediction |= 1 << _bitWidth;
        }
        int8_t score = ((_bitWidth + (_withParity ? 1 : 0)) >> 1) - hammingWeight(newData ^ prediction);
        _bin.add(i, score);
    }
}

uint8_t Binner::bcd2int(uint8_t bcd)
{
    //ret = highnibble * 10 + lownibble = highnibble*8 + highnibble*2 + lownibble
    uint8_t ret = bcd & 0xF;
    bcd &= 0xF0;
    return (bcd >> 1) + (bcd >> 3) + ret;
}

//https://www.electronicdesign.com/displays/easily-convert-decimal-numbers-their-binary-and-bcd-formats
uint8_t Binner::int2bcd(uint8_t hex)
{
    uint8_t highNibble = hex / 10;
    return (highNibble << 2) + (highNibble << 1) + hex;
}

bool Binner::parityOdd(uint8_t x)
{
    x ^= x >> 4;
    x &= 0xf;
    return (0x6996 >> x) & 1;
}

//https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer#109025
int Binner::hammingWeight(int i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


// binOffset = 0 to _datasize - 1;
uint8_t Binner::getValueInRange(uint8_t binOffset)
{
    return _lowestValue + ((binOffset + _currentTick) % _bin.size());
}