#include "CValveCtrl.h"
#include <safeout.h>
#ifdef __arm__
#include <wiringPi.h>
#endif

volatile bool CValveCtrl::m_terminate = false;


CValveCtrl::CValveCtrl()
{

    dbg::sout << dbg::dbg << "Init valve HW" << dbg::endl;
    #ifdef __arm__
    if(wiringPiSetup() == -1)
    {
        dbg::sout << dbg::err << "Failed to init GPIO HW" << dbg::endl;
    }
    else
    {
        pinMode(2, OUTPUT);
        pinMode(3, OUTPUT);
        digitalWrite(2, LOW);
        digitalWrite(3, LOW);
    }
    #endif
}

CValveCtrl::~CValveCtrl()
{
    dbg::sout << dbg::dbg << "Deinit valve HW" << dbg::endl;
    #ifdef __arm__
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    #endif
}

void CValveCtrl::turn(bool on)
{
    dbg::sout << dbg::info << "Switch valve HW " << (on?"on":"off") << dbg::endl;
    #ifdef __arm__
    if(on)
    {
        digitalWrite (2, HIGH);
        digitalWrite (3, LOW);
    }
    else
    {
        digitalWrite (2, LOW);
        digitalWrite (3, HIGH);
    }
    #endif
}
