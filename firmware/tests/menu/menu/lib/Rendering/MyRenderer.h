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
#include "TimeNumericMenuItem.h"
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

class TimeNumericMenuItem;
class TimeMenu;

class MyRenderer : public MenuComponentRenderer
{
  public:
    const byte PCD8544_CHAR_HEIGHT = 8;
    MyRenderer(Adafruit_PCD8544 *lcdModule);
    void render(Menu const &menu) const;
    void render_menu_item(MenuItem const &menu_item) const;
    void render_back_menu_item(BackMenuItem const &menu_item) const;
    void render_numeric_menu_item(NumericMenuItem const &menu_item) const;
    void render_time_numeric_menu_item(TimeNumericMenuItem const &menu_item) const;
    void render_menu(Menu const &menu) const;
    void render_time_menu(TimeMenu const &menu) const;

  private:
    Adafruit_PCD8544 *_pLcd;
};
