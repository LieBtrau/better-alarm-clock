#pragma once
#include "Arduino.h"

typedef void (*voidFuncPtrByte)(byte b);
typedef byte (*byteFuncPtrVoid)(void);

class KeyboardScan
{
public:
    KeyboardScan(){};
    void init(voidFuncPtrByte writePinModes, voidFuncPtrByte writePullups);
    bool updateKeys(voidFuncPtrByte writeGpio, byteFuncPtrVoid readGpio);
    void setCallback_keyChange(voidFuncPtrByte keyChanged);
    bool isKeyDown(byte keyNr);
private:
    byte curRow = 0;
    word keyState = 0;
    voidFuncPtrByte _keyChanged = nullptr;
};