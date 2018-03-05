#ifndef RADIOINTERFACEI2C_H
#define RADIOINTERFACEI2C_H
#include "radiointerface.h"

class RadioInterfaceI2c : public RadioInterface
{
public:
    RadioInterfaceI2c();
    void init();
    bool isDetected(byte address);
    bool send(byte address, byte* data, byte length);
    bool receive(byte address, byte* data, byte length);
    bool sendReceive(byte address, byte* wData, byte wLength, byte* rData, byte rLength);
private:
    bool send(byte address, byte* data, byte length, bool bStop);
};

#endif // RADIOINTERFACEI2C_H
