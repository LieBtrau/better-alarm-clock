#include "Visuals.h"
//#include "fontBig.h"

MenuOut::MenuOut()
{
    _flashTimer.start(FLASH_INTERVAL, AsyncDelay::MILLIS);
}

/* forceRender can be used to force a show() when the underlying parameter hasn't changed.  This behavior is useful in the case of flashing.
 * returns true if content has changed since last call, which would imply a redraw is needed.
 */
bool MenuOut::render(bool forceRender)
{
    if (_flashMode && _flashTimer.isExpired())
    {
        _flashTimer.restart();
        setVisible(flashVisible);
        flashVisible = !flashVisible;
    }
    if (updateNeeded || forceRender)
    {
        if (visible)
        {
            show();
        }
        else
        {
            hide();
        }
    }
    bool retVal = updateNeeded;
    updateNeeded = false;
    return retVal;
}

void MenuOut::setVisible(bool toVisible)
{
    if (toVisible == visible)
    {
        return;
    }
    visible = toVisible;
    updateNeeded = true;
}

void MenuOut::setFlashMode(bool isFlashing)
{
    _flashMode = isFlashing;
}
