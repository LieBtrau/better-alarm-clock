/*
 * An example of a custom NumericMenuItem.
 * It tries to display some ASCII graphics in edit mode.
 * This can be useful if you want to give the end user an overview of the value limits.
 * 
 * Copyright (c) 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#pragma once

#include <MenuSystem.h>
#include "MyRenderer.h"

class MyRenderer;

class TimeMenu : public Menu
{
  public:
    TimeMenu(const char *name);
    void render(MenuComponentRenderer const &renderer) const;
};

class TimeNumericMenuItem : public NumericMenuItem
{
  public:
    TimeNumericMenuItem(const char *name, float value,
                        float minValue, float maxValue, byte xOffset, float increment = 1.0,
                        FormatValueFnPtr on_format_value = nullptr);
    virtual void render(MenuComponentRenderer const &renderer) const;
    byte getXoffset() const;

  private:
    byte _xOffset;
};

class HoursNumericMenuItem : public TimeNumericMenuItem
{
  public:
    HoursNumericMenuItem(byte xOffset);
};

class MinutesNumericMenuItem : public TimeNumericMenuItem
{
  public:
    MinutesNumericMenuItem(byte xOffset);
};
