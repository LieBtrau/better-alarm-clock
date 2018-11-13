#pragma once
#include "Arduino.h"
class Bin
{
public:
    Bin(uint8_t dataSize, int8_t initVal=0);
    void add(uint8_t index, int8_t N);
    uint8_t maximum(int8_t threshold);
    uint8_t size();
    uint8_t getUnsigned(uint8_t index);
private:
    void bounded_increment(uint8_t index, int8_t N);
    int8_t* _pData=nullptr;
    uint8_t _dataSize=0;
};