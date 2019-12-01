#include "KeyboardScan.h"

void KeyboardScan::init(voidFuncPtrByte writePinModes, voidFuncPtrByte writePullups)
{
    //four lowest bits make up the columns, which are set as output
    //four highest bits make up the rows, which are set as inputs
    writePinModes(0xF0); // output = setting bit to 0
    writePullups(0xF0);  // pull up enabled = 1
}

// return true if a button has been pressed or released
bool KeyboardScan::updateKeys(voidFuncPtrByte writeGpio, byteFuncPtrVoid readGpio)
{
    curRow = curRow + 1 > 3 ? 0 : curRow + 1;
    //Row control
    byte rowCmd = 0x0F & (~(1 << curRow)); //make one of the four lowest bits low, another one on each function call
    writeGpio(rowCmd);
    //Column reading
    byte colState = readGpio() >> 4;
    //Copy four read buttons into the keystate register
    word clear = ~(0xF << (curRow << 2)); // prepare intermediate value to clear the status of the current 4 buttons in the keystate
    word prevKeyState = keyState;
    keyState &= clear;                     // clear the status of the current 4 buttons in the keystate
    keyState |= colState << (curRow << 2); // copy the status of the four buttons into the keystate register
    if (prevKeyState == keyState)
    {
        return false;
    }
    word diffkey = keyState ^ prevKeyState;
    for (byte i = 0; i < 16; i++)
    {
        if (bitRead(diffkey, i))
        {
            if (bitRead(keyState, i))
            {
                lastReleasedKey = i;
                isReleasedKeyValid = true;
                if (_keyReleased != nullptr)
                {
                    _keyReleased(i);
                }
            }
            else
            {
                if (_keyPressed != nullptr)
                {
                    _keyPressed(i);
                }
            }
        }
    }
    return true;
}

void KeyboardScan::setCallback_keyPressed(voidFuncPtrByte keyPressed)
{
    _keyPressed = keyPressed;
}

void KeyboardScan::setCallback_keyReleased(voidFuncPtrByte keyReleased)
{
    _keyReleased = keyReleased;
}

bool KeyboardScan::isKeyDown(byte keyNr)
{
    return bitSet(keyState, keyNr);
}

bool KeyboardScan::getLastKeyReleased(byte *key)
{
    if (!isReleasedKeyValid)
    {
        return false;
    }
    *key = lastReleasedKey;
    isReleasedKeyValid = false;
    return true;
}