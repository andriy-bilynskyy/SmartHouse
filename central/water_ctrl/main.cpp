#include "CValveCtrl.h"
#include <CDBHandler.h>
#include <safeout.h>
#include <signal.h>
#include <unistd.h>

void intHandler(int dummy)
{
    static_cast<void>(dummy);
    CValveCtrl::m_terminate = true;
}

int main(void)
{
    dbg::sout.setDbgLevel(dbg::DBG_DEBUG); // (dbg::DBG_INFO);
    dbg::sout << dbg::warn << "Water valve control started" << dbg::endl;
    static_cast<void>(signal(SIGINT, intHandler));
    static_cast<void>(signal(SIGHUP, intHandler));
    static_cast<void>(signal(SIGQUIT, intHandler));
    static_cast<void>(signal(SIGTSTP, intHandler));
    static_cast<void>(signal(SIGTERM, intHandler));
    CDBHandler db;
    db.add_log("WATER VALVE", "Water valve control started.");
    CValveCtrl valve;
    while(!CValveCtrl::m_terminate)
    {
        if(!db.check_water_fault(db.get_water_valve()))
        {
            if(db.get_water_valve())
            {
                db.set_water_valve(false);
                dbg::sout << dbg::warn << "Water closed because of fault" << dbg::endl;
                db.add_log("WATER VALVE", "Water closed because of fault.");
            }
        }
        valve.turn(db.get_water_valve());
        if(db.need_water_scheduled_turn())
        {
            dbg::sout << dbg::warn << "Need schedule turn" << dbg::endl;
            if(db.get_water_valve())
            {
                db.set_water_valve(false);
                valve.turn(false);
                dbg::sout << dbg::warn << "Water closed because of schedule" << dbg::endl;
                db.add_log("WATER VALVE", "Water closed because of schedule.");
                sleep(10);
                db.set_water_valve(true);
                valve.turn(true);
                dbg::sout << dbg::warn << "Water opened because of schedule" << dbg::endl;
                db.add_log("WATER VALVE", "Water opened because of schedule.");
                sleep(10);
            }
        }
        else
        {
            sleep(1);
        }
    }
    static_cast<void>(signal(SIGINT, SIG_DFL));
    static_cast<void>(signal(SIGHUP, SIG_DFL));
    static_cast<void>(signal(SIGQUIT, SIG_DFL));
    static_cast<void>(signal(SIGTSTP, SIG_DFL));
    static_cast<void>(signal(SIGTERM, SIG_DFL));
    dbg::sout << dbg::warn << "Water valve control stopped" << dbg::endl;
    db.add_log("WATER VALVE", "Water valve control stopped.");
    return 0;
}
