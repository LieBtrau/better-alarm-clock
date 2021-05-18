#pragma once

#include "Tactiles.h"
#include "pins.h"

typedef enum
{
    KEY_ROTENC = 1,
    KEY_SUNRISE = 4360,
    KEY_VOLUME = 4356,
    KEY_SONGCHOICE = 4354,
    KEY_ALARM = 4353,
    KEY_MONDAY = 257,
    KEY_TUESDAY = 258,
    KEY_WEDNESDAY = 260,
    KEY_THURSDAY = 513,
    KEY_FRIDAY = 514,
    KEY_SATURDAY = 516,
    KEY_SUNDAY = 520
} KEY_CODE;

bool setupInputs(SX1509* io1, SX1509* io2);
//bool inputsChanged();
bool isRotaryEncoderChanged(RotaryEncoder_Tactiles::DIRECTION &dir);
bool isButtonChanged(KEY_CODE &key);