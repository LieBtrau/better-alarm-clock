#include "at24c.h"

AT24C::AT24C(DEVICES device, byte i2cAddress, TwoWire *i2cBus) : _device(device), _i2cAddress(i2cAddress), _i2cBus(i2cBus) {}


bool AT24C::i2c_setMemAddress(word address, word length)
{
    switch (_device)
    {
    case AT24C02:
        if (address + length > AT24C02_SIZE)
        {
            return false;
        }
        break;
    default:
        break;
    }
    _i2cBus->beginTransmission(_i2cAddress);
    switch (_device)
    {
    case AT24C02:
        _i2cBus->write((byte)(address & 0xFF));
        break;
    default:
        _i2cBus->write((int)(address >> 8));
        _i2cBus->write((int)(address & 0xFF));
        break;
    }
    return true;
}

bool AT24C::i2c_byteRead(word address, byte &data)
{
    if (!i2c_setMemAddress(address, 0))
    {
        return false;
    }
    _i2cBus->endTransmission(false);
    if (!_i2cBus->requestFrom((byte)_i2cAddress, (byte)1))
    {
        return false;
    }
    if (_i2cBus->available())
    {
        data = _i2cBus->read();
        return true;
    }
    return false;
}

bool AT24C::i2c_byteWrite(word address, byte data)
{
    if (!i2c_setMemAddress(address, 1))
    {
        return false;
    }
    _i2cBus->write(data);
    if (_i2cBus->endTransmission())
    {
        return false;
    };
    delay(EEPROM_DELAY);
    return true;
}

// \return Number of bytes read
word AT24C::i2c_readBytes(word address, byte *data, word length)
{
    if (!i2c_setMemAddress(address, 0))
    {
        return 0;
    }
    _i2cBus->endTransmission(false);
    if (!_i2cBus->requestFrom(_i2cAddress, length))
    {
        return 0;
    }
    if(!_i2cBus->available())
    {
        return 0;
    }
    return _i2cBus->readBytes(data, length);
}

// \brief Writes byte array to EEPROM.  Uses page writes to upgrade speed.
// //https://github.com/cyberp/AT24Cx/blob/master/AT24CX.cpp
bool AT24C::i2c_pageWrite(word address, const byte *data, word length)
{
    byte page_size = 0;
    
    switch (_device)
    {
    case AT24C02:
    default:
        page_size = 8;      // 32 pages of 8 bytes : 8 bytes can be written at once as long as the address aligns with the page boundaries
        break;
    }
    word bytesLeft = length;
    while (bytesLeft)
    {
        word bytes_written = length - bytesLeft;
        word page_write_max = page_size - ((address + bytes_written) % page_size);
        byte bytes_to_write = min((word)min(bytesLeft, (word)page_size), page_write_max);
        if (!i2c_setMemAddress(address + bytes_written, bytes_to_write))
        {
            return false;
        }
        _i2cBus->write(&data[bytes_written], bytes_to_write);
        if (_i2cBus->endTransmission())
        {
            return false;
        };
        delay(EEPROM_DELAY);
        bytesLeft -= bytes_to_write;
    }
    return true;
}

#ifndef ARDUINO_ARCH_AVR
//http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__util__crc_1ga1c1d3ad875310cbc58000e24d981ad20.html
word _crc_ccitt_update(word crc, byte data)
{
    data ^= crc & 0xFF;
    data ^= data << 4;

    return ((((uint16_t)data << 8) | (crc >> 8)) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
}
#endif
