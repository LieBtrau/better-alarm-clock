#include "dcf77receiver.h"

static void update();
static int numberOfSetBits(unsigned long i);
static Dcf77Receiver *pDcf;
unsigned long intervalTime = 0;
Dcf77Receiver::Dcf77Receiver(byte dcfPin) : _dcfPin(dcfPin)
{
    pDcf = this;
}

void Dcf77Receiver::init()
{
    pinMode(_dcfPin, INPUT_PULLUP);
#ifdef ARDUINO_ARCH_AVR
    // Set up Timer1 for interrupt:
    TCCR1A = _BV(WGM11);                          // Mode 14 (fast PWM), OC1A off
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // Mode 14, no prescale
    ICR1 = 10000;                                 //160Hz row refresh rate
    TIMSK1 |= _BV(TOIE1);                         // Enable Timer1 interrupt
    sei();                                        // Enable global interrupts
#elif ARDUINO_ARCH_STM32F1
    Timer2.setChannel1Mode(TIMER_OUTPUTCOMPARE);
    Timer2.setPeriod(3125);          // in microseconds = 320Hz
    Timer2.setCompare(TIMER_CH1, 1); // overflow might be small
    Timer2.attachInterrupt(TIMER_CH1, update);
#endif
}

bool Dcf77Receiver::getLocalTime(Chronos::DateTime &localTime)
{
    if (millis() - intervalTime > 500)
    {
        intervalTime = millis();
        //show status
    }
    if (!_bufReady)
    {
        return false;
    }
    _bufReady = false;
    return true;
}

void Dcf77Receiver::sampleDcf()
{
    _cycleCtr++;
    //Bit FIFO : New bits shifted in on LSB.LSb
    for (int i = RXBUFSIZE - 1; i > 0; i--)
    {
        _rxBuf[i] <<= 1;
        if (_rxBuf[i - 1] & 0x80000000)
        {
            _rxBuf[i] |= 1;
        }
    }
    _rxBuf[0] <<= 1;
    _rxBuf[0] |= digitalRead(_dcfPin);
    //    for(byte i=0;i<10;i++)
    //    {
    //        Serial1.print(_rxBuf[9-i], BIN);Serial1.print(" ");
    //    }
    //    Serial1.println();
    //Serial1.println(digitalRead(_dcfPin));//For Arduino IDE Tools->Serial Plotter

    //Correlation
    //800ms HIGH, followed by 100ms low is repeated every second, except for the last second of the minute.
    //Try to sync on that pattern.
    //DCF77 : 10 chips/second : 0 = 2L/8H, 1 = 1L/9H
    //This code samples at 320Hz, so that 32bits/chip.  So chip size = size of unsigned long.  This makes correlation operation easy.
    long correlation = 0;
    for (int i = 1; i < RXBUFSIZE; i++)
    {
        if (i % 10)
        {
            correlation += numberOfSetBits(_rxBuf[i] ^ (i % 10 == 1 ? 0x00000000UL : 0XFFFFFFFF));
        }
    }
    // Serial1.print(digitalRead(_dcfPin)<<3);
    // Serial1.print(",");
    // Serial1.print(correlation);
    //Exponential smoothing: y[n] = alfa * x[n] + (1-alfa) * y[n-1], with 0 < alfa < 1
    if (!((_subSampling++) & 0x3F))
    {
        _smoothCorrelation = (ALFA * correlation + (256 - ALFA) * _smoothCorrelation) >> 8;
    }
    // Serial1.print(",");
    // First order derivative : y[n] = x[n] - x[n-1];
    _deltaCorrelation = correlation - _prevCorrelation;
    _prevCorrelation = correlation;
    //Amplitude threshold
    if (correlation < (_smoothCorrelation >> 1))
    {
        //Find zerocrossing on downward correlation slope
        if (_minSmoothEdge < 0 && _deltaCorrelation > 0)
        {
            _minSmoothEdge = 0;
            if (_cycleCtr > _pllReg)
            {
                _pllReg++;
            }
            if (_cycleCtr < _pllReg)
            {
                _pllReg--;
            }
            _cycleCtr = 0;
        }
        if (_deltaCorrelation < _minSmoothEdge)
        {
            _minSmoothEdge = _deltaCorrelation;
        }
    }
    if(_cycleCtr==_pllReg)
    {
        //Save data now.  It will be needed in case pulse doesn't come in due time.
        _backupData = numberOfSetBits(_rxBuf[0]);
    }
    if(_cycleCtr > _pllReg+20  || !_cycleCtr)
    {
        Serial1.print(millis()-_prevTimeStamp);
        _prevTimeStamp=millis();
        Serial1.print(",");
        Serial1.print(_pllReg);
        Serial1.print(",");
        Serial1.println(numberOfSetBits(_cycleCtr ? _backupData :_rxBuf[0]) < 16 ? 1 : 0);
        _cycleCtr=0;
    }
 }

#ifdef ARDUINO_ARCH_AVR
ISR(TIMER1_OVF_vect, ISR_BLOCK)
{                  // ISR_BLOCK important -- see notes later
    TIFR1 |= TOV1; // Clear Timer1 interrupt flag
#elif ARDUINO_ARCH_STM32F1
void update()
{
#endif
    pDcf->sampleDcf();
}

//https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer#109025
int numberOfSetBits(unsigned long i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
