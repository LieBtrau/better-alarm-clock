#include "radiointerfacei2c.h"

#include <Wire.h>
#ifdef ARDUINO_STM_NUCLEO_F103RB
//SCL = SCL/D15
//SDA = SDA/D14
HardWire HWire(1, I2C_REMAP);// | I2C_BUS_RESET); // I2c1
#else
#define HWire Wire
#endif

RadioInterfaceI2c::RadioInterfaceI2c()
{

}

void RadioInterfaceI2c::init()
{
    HWire.begin();
}

bool RadioInterfaceI2c::isDetected(byte address)
{
    HWire.beginTransmission(address);
    return HWire.endTransmission()==0;
//    for(byte i=1;i<128;i++)
//    {
//        HWire.beginTransmission(i);
//        if(HWire.endTransmission()==0)
//        {
//            HWire.endTransmission();
//            Serial.println(i, HEX);
//        }
//    }
}

bool RadioInterfaceI2c::send(byte address, byte* data, byte length)
{
    return send(address, data, length, true);
}

bool RadioInterfaceI2c::receive(byte address, byte* data, byte length)
{
    HWire.requestFrom(address, length);
    if(HWire.available()!=length)
    {
        return false;
    }
    for(byte i=0;i<length;i++)
    {
        data[i]=HWire.read();
    }
    return true;
}

bool RadioInterfaceI2c::sendReceive(byte address, byte* wData, byte wLength, byte* rData, byte rLength)
{
    if(!send(address, wData, wLength, false))
    {
        return false;
    }
    return receive(address, rData, rLength);
}

bool RadioInterfaceI2c::send(byte address, byte* data, byte length, bool bStop)
{
    HWire.beginTransmission(address);
    for(byte i=0;i<length;i++)
    {
        HWire.write(data[i]);
    }
    return HWire.endTransmission(bStop)==0;
}


