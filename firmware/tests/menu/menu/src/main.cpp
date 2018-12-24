/*
 * pcd8544_nav.ino - Example code using the menu system library
 *
 * This example shows using the menu system with a pcd8544 LCD display
 * (controlled over serial).
 *
 * Copyright (c) 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <MyRenderer.h>

Adafruit_PCD8544 lcd = Adafruit_PCD8544(PB0, PA4, PB1);
MyRenderer my_renderer(&lcd);

// Menu callback function

void on_item1_selected(MenuComponent *p_menu_component)
{
    lcd.setCursor(0, 2 * my_renderer.PCD8544_CHAR_HEIGHT);
    lcd.print("Item1 Selectd");
    lcd.display();
    delay(1500); // so we can look the result on the LCD
}

void on_item2_selected(MenuComponent *p_menu_component)
{
    lcd.setCursor(0, 2 * my_renderer.PCD8544_CHAR_HEIGHT);
    lcd.print("Item2 Selectd");
    lcd.display();
    delay(1500); // so we can look the result on the LCD
}

void on_item3_selected(MenuComponent *p_menu_component)
{
    lcd.setCursor(0, 2 * my_renderer.PCD8544_CHAR_HEIGHT);
    lcd.print("Item3 Selectd");
    lcd.display();
    delay(1500); // so we can look the result on the LCD
}

// Menu variables
MenuSystem ms(my_renderer);
MenuItem mm_mi1("Lvl1-Item1(I)", &on_item1_selected);
MenuItem mm_mi2("Lvl1-Item2(I)", &on_item2_selected);
TimeMenu mu1("Alarmtime");
HoursNumericMenuItem hours(0);
MinutesNumericMenuItem minutes(42);

void serial_print_help()
{
    Serial.println("***************");
    Serial.println("z: go to previous item (up)");
    Serial.println("s: go to next item (down)");
    Serial.println("q: go back (right)");
    Serial.println("d: select \"selected\" item");
    Serial.println("?: print this help");
    Serial.println("h: print this help");
    Serial.println("***************");
}

void serial_handler()
{
    char inChar;
    if ((inChar = Serial.read()) > 0)
    {
        switch (inChar)
        {
        case 'z': // Previus item
            ms.prev();
            ms.display();
            break;
        case 's': // Next item
            ms.next();
            ms.display();
            break;
        case 'q': // Back pressed
            ms.back();
            ms.display();
            break;
        case 'd': // Select pressed
            ms.select(true);
            ms.display();
            break;
        case '?':
        case 'h': // Display help
            serial_print_help();
            break;
        default:
            break;
        }
    }
}

// Standard arduino functions

void setup()
{
    Serial.begin(115200);

    lcd.begin();
    lcd.setContrast(60);

#ifdef HIDE_SPLASH
    lcd.clearDisplay();
#else
    lcd.display();
    delay(2000);
#endif

    lcd.setTextSize(1);

    serial_print_help();

    ms.get_root_menu().add_item(&mm_mi1);
    ms.get_root_menu().add_item(&mm_mi2);
    ms.get_root_menu().add_menu(&mu1);
    mu1.add_item(&hours);
    mu1.add_item(&minutes);
    ms.display();
}

void loop()
{
    serial_handler();
}
