#include "TimeNumericMenuItem.h"

const String format_twoDigit(const float value)
{
    String str1 = "00" + String((int)value);
    return str1.substring(str1.length() - 2);
}

TimeMenu::TimeMenu(const char *name) : Menu(name) {}
void TimeMenu::render(MenuComponentRenderer const &renderer) const
{
    MyRenderer const &my_renderer = static_cast<MyRenderer const &>(renderer);
    my_renderer.render_time_menu(*this);
}

TimeNumericMenuItem::TimeNumericMenuItem(
    const char *name, float value, float minValue,
    float maxValue, byte xOffset, float increment, FormatValueFnPtr on_format_value)
    : NumericMenuItem(name, nullptr, value, minValue, maxValue, increment,
                      on_format_value),
      _xOffset(xOffset) {}
HoursNumericMenuItem::HoursNumericMenuItem(byte xOffset) : TimeNumericMenuItem("H", 7, 0, 23, xOffset, 1, format_twoDigit) {}
MinutesNumericMenuItem::MinutesNumericMenuItem(byte xOffset) : TimeNumericMenuItem("M", 0, 0, 55, xOffset, 5, format_twoDigit) {}

byte TimeNumericMenuItem::getXoffset() const
{
    return _xOffset;
}

void TimeNumericMenuItem::render(MenuComponentRenderer const &renderer) const
{
    MyRenderer const &my_renderer = static_cast<MyRenderer const &>(renderer);
    my_renderer.render_time_numeric_menu_item(*this);
}
