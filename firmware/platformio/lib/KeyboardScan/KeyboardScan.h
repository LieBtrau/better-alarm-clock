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
    void setCallback_keyReleased(voidFuncPtrByte keyReleased);
    void setCallback_keyPressed(voidFuncPtrByte keyPressed);
    bool isKeyDown(byte keyNr);
    bool getLastKeyReleased(byte *key);

private:
    byte curRow = 0;
    word keyState = 0xFFFF; //0xFFFF instead of 0x0000 prevents false keypresses at startup
    voidFuncPtrByte _keyPressed = nullptr;
    voidFuncPtrByte _keyReleased = nullptr;
    bool isReleasedKeyValid = false;
    byte lastReleasedKey = 0;
};