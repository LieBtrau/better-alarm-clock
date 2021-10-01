#include "Arduino.h"
#include "Wire.h"
#pragma once

//#define DEBUG

class AT24C
{
public:
    typedef enum
    {
        AT24C02
    } DEVICES;
    AT24C(DEVICES device, byte i2cAddress, TwoWire *i2cBus);
    template <typename T> 
    bool write(int ee, const T &t);
    template <typename T> 
    bool read(int ee, T &t);

private:
    bool i2c_pageWrite(word address, const byte *data, word length);
    bool i2c_setMemAddress(word address, word length);
    word i2c_readBytes(word address, byte *data, word length);
    bool i2c_byteWrite(word address, byte data);
    bool i2c_byteRead(word address, byte &data);
    const byte EEPROM_DELAY = 5;
    const word AT24C02_SIZE = 256;
    DEVICES _device;
    byte _i2cAddress;
    TwoWire *_i2cBus;
};

#ifdef ARDUINO_ARCH_AVR
#include <util/crc16.h>
#else
//http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__util__crc_1ga1c1d3ad875310cbc58000e24d981ad20.html
word _crc_ccitt_update(word crc, byte data);
#endif

/* 
 * Template definitions (must be placed in header file, not in source files)
 */
template <typename T> 
bool AT24C::read(int ee, T &t)
{
    word crcIn = 0xFFFF;
    uint8_t *ptr = (uint8_t *)&t;
    if (!i2c_readBytes(ee, ptr, sizeof(T)))
    {
        return false;
    }
    for (int count = sizeof(T); count; --count)
    {
        crcIn = _crc_ccitt_update(crcIn, *ptr++);
    }
    word crcRead=0;
    ee += sizeof(T);
    if (!i2c_readBytes(ee, (byte*)&crcRead, 2))
    {
        return false;
    }
#ifdef DEBUG
    Serial.print("Read CRC: ");
    Serial.println(crcRead, HEX);
    Serial.print("Calculated CRC: ");
    Serial.println(crcIn, HEX);
#endif
    return crcRead == crcIn;
}

template <typename T> 
bool AT24C::write(int ee, const T &t)
{
    word crc = 0xFFFF;
    const uint8_t *ptr = (const uint8_t *)&t;
    for (int count = sizeof(T); count; --count)
    {
        crc = _crc_ccitt_update(crc, *ptr++);
    }
    if (!i2c_pageWrite(ee, (const uint8_t *)&t, sizeof(T)))
    {
        return false;
    }
    ee += sizeof(T);
#ifdef DEBUG    
    Serial.print("write CRC: ");Serial.println(crc, HEX);
#endif
    return i2c_pageWrite(ee, (byte*)&crc, 2);
}
