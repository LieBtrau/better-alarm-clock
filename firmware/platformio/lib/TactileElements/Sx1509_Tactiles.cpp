#include "Tactiles.h"

static Sx1509_Tactiles *pSt = nullptr;

void irq_keyPad()
{
    pSt->processIrq();
}

//Will only be called as long a key is pressed.
void Sx1509_Tactiles::processIrq()
{
    int keyData = _sx->readKeypad();
    if (keyData != _previousKeyData)
    {
        _newKey = true;
    }
    _keyReleaseTimeout.restart();
    _previousKeyData = keyData;
}

Sx1509_Tactiles::Sx1509_Tactiles(SX1509 *sx, byte pinIrq) : _sx(sx), _pin_irq(pinIrq), _keyReleaseTimeout(100, AsyncDelay::MILLIS)
{
    pSt = this;
}

void Sx1509_Tactiles::init(byte keyRows, byte keyCols)
{
    // Scan time range: 1-128 ms, powers of 2
    const byte scanTime = 16; // Scan time per row, in ms
    // Debounce time range: 0.5 - 64 ms (powers of 2)
    const byte debounceTime = 8; // Debounce time
    // Sleep time range: 128 ms - 8192 ms (powers of 2) 0=OFF
    const byte sleepTime = 128;
    // Scan time must be greater than debounce time!
    _sx->keypad(keyRows, keyCols, sleepTime, scanTime, debounceTime);
    pinMode(_pin_irq, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_pin_irq), irq_keyPad, FALLING);
}

//\return true when new key pressed or when a key is released
bool Sx1509_Tactiles::isChanged()
{
    if (_newKey)
    {
        _newKey = false;
        return true;
    }
    if(_keyReleaseTimeout.isExpired() && _previousKeyData!=0)
    {
        _previousKeyData=0; //so that the same key can be pressed again.
        return true;
    }
    return false;
}

//\return keycode of pressed key or 0 when no keys are pressed
int Sx1509_Tactiles::getValue()
{
    return _previousKeyData;
}