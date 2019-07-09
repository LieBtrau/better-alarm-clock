#include "screenManager.h"

bool ScreenManager::addItem(MenuOut *item)
{
    MenuOut **_itemListLonger = (MenuOut **)realloc(_itemList, ++_nrOfItems * sizeof(MenuOut *));
    if (_itemListLonger == nullptr)
    {
        free(_itemList);
        return false;
    }
    _itemList = _itemListLonger;
    _itemList[_nrOfItems - 1] = item;
    return true;
}

bool ScreenManager::render(bool forceRender)
{
    bool redrawNeeded = false;
    for (byte i = 0; i < _nrOfItems; i++)
    {
        redrawNeeded |= _itemList[i]->render(forceRender);
    }
    return redrawNeeded;
}

void ScreenManager::setVisible(bool isVisible)
{
    for (byte i = 0; i < _nrOfItems; i++)
    {
        _itemList[i]->setVisible(isVisible);
    }
}
