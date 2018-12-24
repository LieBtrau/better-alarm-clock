#include "MyRenderer.h"

MyRenderer::MyRenderer(Adafruit_PCD8544 *lcdModule) : _pLcd(lcdModule)
{
}

void MyRenderer::render(Menu const &menu) const
{
    _pLcd->clearDisplay();
    menu.render(*this);
    menu.get_current_component()->render(*this);
    _pLcd->display();
}

void MyRenderer::render_menu_item(MenuItem const &menu_item) const
{
    _pLcd->setCursor(0, 1 * PCD8544_CHAR_HEIGHT);
    _pLcd->print(menu_item.get_name());
}

void MyRenderer::render_back_menu_item(BackMenuItem const &menu_item) const
{
    _pLcd->setCursor(0, 1 * PCD8544_CHAR_HEIGHT);
    _pLcd->print(menu_item.get_name());
}

void MyRenderer::render_numeric_menu_item(NumericMenuItem const &menu_item) const
{
    _pLcd->setCursor(0, 1 * PCD8544_CHAR_HEIGHT);
    String buffer;

    buffer = menu_item.get_name();
    buffer += menu_item.has_focus() ? '<' : '=';
    buffer += menu_item.get_formatted_value();

    if (menu_item.has_focus())
        buffer += '>';

    Serial.print(buffer);
    _pLcd->print(buffer);
}

void MyRenderer::render_time_numeric_menu_item(TimeNumericMenuItem const &menu_item) const
{
    _pLcd->setCursor(menu_item.getXoffset(), 1 * PCD8544_CHAR_HEIGHT);
    String buffer;

    if (menu_item.is_current())
    {
        buffer += '[';
    }
    else if (menu_item.has_focus())
    {
        buffer += '<';
    }
    buffer += menu_item.get_formatted_value();

    if (menu_item.has_focus())
    {
        buffer += '>';
    }
    else if (menu_item.is_current())
    {
        buffer += ']';
    }
    _pLcd->print(buffer);
}

void MyRenderer::render_menu(Menu const &menu) const
{
    _pLcd->setCursor(0, 0 * PCD8544_CHAR_HEIGHT);
    _pLcd->print(menu.get_name());
}

void MyRenderer::render_time_menu(TimeMenu const &menu) const
{
    render_menu(menu);
    byte nrOfTimeComponents = menu.get_num_components();
    if (nrOfTimeComponents != 2)
    {
        return;
    }
    menu.get_menu_component(0)->render(*this);
    menu.get_menu_component(1)->render(*this);
    _pLcd->display();
}
