/********************
Arduino generic menu system

Rui Azevedo - ruihfazevedo(@rrob@)gmail.com

output: Serial
input: Serial
mcu: stm32f103 (blue pill)

Used keys : 
 + up
 - down
 * click
*/

#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

using namespace Menu;

#define LEDPIN PC13
HardwareSerial* sp=&Serial;

result showEvent(eventMask e,navNode& nav,prompt& item) {
    sp->print("event: ");
    sp->println(e);
    return proceed;
}

byte volume=1;
byte* pVol=&volume;
int alarm1enabled=LOW;
bool alarm1Monday=false,
alarm1Tuesday=false,
alarm1Wednesday=false,
alarm1Thursday=false,
alarm1Friday=false,
alarm1Saturday=false,
alarm1Sunday=false;
int alarm1type=1;
uint16_t hour=10;
uint16_t minute=0;
int alarm1repeater=1;
byte alarm1tone=0;
byte brightness=1;

TOGGLE(alarm1enabled,mnuAlarmActive,"Active: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
       ,VALUE("Yes",HIGH,doNothing,noEvent)
       ,VALUE("No",LOW,doNothing,noEvent)
       );


TOGGLE(alarm1Monday,tglAlarmMon,"Mon: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
       ,VALUE("On",true,doNothing,noEvent)
       ,VALUE("Off",false,doNothing,noEvent)
       );

TOGGLE(alarm1Tuesday,tglAlarmTue,"Tue: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
       ,VALUE("On",true,doNothing,noEvent)
       ,VALUE("Off",false,doNothing,noEvent)
       );

TOGGLE(alarm1Wednesday,tglAlarmWed,"Wed: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
       ,VALUE("On",true,doNothing,noEvent)
       ,VALUE("Off",false,doNothing,noEvent)
       );

TOGGLE(alarm1Thursday,tglAlarmThu,"Thu: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
       ,VALUE("On",true,doNothing,noEvent)
       ,VALUE("Off",false,doNothing,noEvent)
       );

TOGGLE(alarm1Friday,tglAlarmFri,"Fri: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
       ,VALUE("On",true,doNothing,noEvent)
       ,VALUE("Off",false,doNothing,noEvent)
       );

TOGGLE(alarm1Saturday,tglAlarmSat,"Sat: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
       ,VALUE("On",true,doNothing,noEvent)
       ,VALUE("Off",false,doNothing,noEvent)
       );

TOGGLE(alarm1Sunday,tglAlarmSun,"Sun: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
       ,VALUE("On",true,doNothing,noEvent)
       ,VALUE("Off",false,doNothing,noEvent)
       );

SELECT(alarm1repeater,mnuAlarmRepeat,"Repeat: ",doNothing,noEvent,wrapStyle
       ,VALUE("Once",1,doNothing,noEvent)
       ,VALUE("Weekly",2,doNothing,noEvent)
       ,VALUE("Daily",3,doNothing,noEvent)
       );

SELECT(alarm1type,mnuAlarm1type,"Type: ",doNothing,noEvent,wrapStyle
       ,VALUE("SND + LIGHT",1,doNothing,noEvent)
       ,VALUE("SND ONLY",2,doNothing,noEvent)
       ,VALUE("LIGHT ONLY",3,doNothing,noEvent)
       );

SELECT(alarm1tone,mnuAlarm1tone,"Tone: ",doNothing,noEvent,wrapStyle
       ,VALUE("Bamboo",0,doNothing,noEvent)
       ,VALUE("Jazz",1,doNothing,noEvent)
       ,VALUE("Guitar",2,doNothing,noEvent)
       ,VALUE("Piano",3,doNothing,noEvent)
       );

PADMENU(padAlarmTime,"Time",doNothing,noEvent,wrapStyle
        ,FIELD(hour,"",":",0,23,5,1,doNothing,noEvent,wrapStyle)
        ,FIELD(minute,"","",0,59,10,1,doNothing,noEvent,wrapStyle)
        );

MENU(alarm1week,"Week",showEvent,anyEvent,wrapStyle
     ,SUBMENU(tglAlarmMon)
     ,SUBMENU(tglAlarmTue)
     ,SUBMENU(tglAlarmWed)
     ,SUBMENU(tglAlarmThu)
     ,SUBMENU(tglAlarmFri)
     ,SUBMENU(tglAlarmSat)
     ,SUBMENU(tglAlarmSun)
     ,EXIT("<Back")
     );

MENU(alarm1,"Alarm1",showAlarm1,anyEvent,wrapStyle
     ,SUBMENU(mnuAlarmActive)
     ,SUBMENU(padAlarmTime)
     ,SUBMENU(mnuAlarmRepeat)
     ,SUBMENU(alarm1week)
     ,SUBMENU(mnuAlarm1tone)
     ,SUBMENU(mnuAlarm1type)
     ,FIELD(*pVol,"Volume:","",1,7,1,1,doNothing,noEvent,noStyle)
     ,FIELD(brightness,"Brightness:","",1,7,1,1,doNothing,noEvent,noStyle)
     ,EXIT("<Back")
     );

result showAlarm1(eventMask e,navNode& nav,prompt& item)
{
    alarm1[3].enabled = (alarm1repeater==2 ? enabledStatus : disabledStatus);
    return proceed;
}

result action1(eventMask e)
{
    sp->print(e);
    sp->println(" action1 executed, proceed menu");
    sp->flush();
    return proceed;
}

result action2(eventMask e, prompt &item)
{
    sp->print(e);
    sp->print(" action2 executed, quiting menu");
    return quit;
}


MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
     ,SUBMENU(alarm1)
     ,EXIT("<Back")
     );

#define MAX_DEPTH 3

MENU_OUTPUTS(out,MAX_DEPTH
             ,SERIAL_OUT(*sp)
             ,NONE//must have 2 items at least
             );

serialIn serial(*sp);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);


//when menu is suspended
result idle(menuOut &o, idleEvent e)
{
    // o.clear();
    switch(e) {
    case idleStart:o.println("suspending menu!");break;
    case idling:o.println("suspended...");break;
    case idleEnd:o.println("resuming menu.");break;
    }
    return proceed;
}

void setup()
{
    pinMode(LEDPIN,OUTPUT);
    digitalWrite(LEDPIN,!alarm1enabled);
    delay(500);
    sp->begin(9600);
    // while(!Serial);//this locks on stm32
    sp->println("menu 4.x test");sp->flush();
    nav.idleTask=idle;//point a function to be used when menu is suspended
    // nav.idleOn();//this menu will start on idle state, press select to enter menu
    //nav.doInput("323");
    nav.timeOut=60;
}

void loop()
{
    nav.poll();
    delay(100);//simulate a delay when other tasks are done
}
