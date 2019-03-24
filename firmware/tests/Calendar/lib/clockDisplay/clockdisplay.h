
class clockdisplay
{
  public:
    virtual void init()=0;
    virtual void showTime(int hour, int minutes) = 0;
    virtual void clearDisplay() = 0;
    virtual void setBrightness(int brightness) = 0;
    virtual void setColon(bool colonOn)=0;
    virtual int getMinutes()=0;
};