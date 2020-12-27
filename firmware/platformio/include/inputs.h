#pragma once

#include "Tactiles.h"
#include "pins.h"

typedef enum
{
    KEY_ROTENC = 1,
    KEY_SUNRISE = 4353,
    KEY_VOLUME = 4354,
    KEY_SONGCHOICE = 4356,
    KEY_ALARM = 4360,
    KEY_MONDAY = 257,
    KEY_TUESDAY = 258,
    KEY_WEDNESDAY = 260,
    KEY_THURSDAY = 264,
    KEY_FRIDAY = 513,
    KEY_SATURDAY = 514,
    KEY_SUNDAY = 516
} KEY_CODE;

bool setupInputs(SX1509* io1, SX1509* io2);
//bool inputsChanged();
bool isRotaryEncoderChanged(RotaryEncoder_Tactiles::DIRECTION &dir);
bool isButtonChanged(KEY_CODE &key);