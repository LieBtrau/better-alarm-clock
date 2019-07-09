#pragma once
#include "Menu.h"

class ScreenManager
{
public:
    bool addItem(MenuOut* item);
    bool render(bool forceRender=false);
    void setVisible(bool isVisible);
private:
    MenuOut** _itemList=nullptr;
    byte _nrOfItems=0;
};