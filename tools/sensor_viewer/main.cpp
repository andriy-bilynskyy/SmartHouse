#include <CBleScanner.h>
#include <safeout.h>
#include <signal.h>


void intHandler(int dummy)
{
    static_cast<void>(dummy);
    dbg::sout << dbg::info << "Terminating..." << dbg::endl;
    CBleSensorScanner::m_terminate_scan = true;
}


class CBleSensorHandler : public CBleSensorScanner
{
private:

    virtual void on_sensor_advertising(std::string addr, std::string sen_type, std::string sen_voltage, std::string sen_value)
    {
        dbg::sout << dbg::info << addr << " : " << sen_type << " : " << sen_voltage << " : "  << sen_value << dbg::endl;
    }
};

int main(void)
{
    dbg::sout.setDbgLevel(dbg::DBG_INFO);
    dbg::sout << dbg::warn << "BLE sensors scanner started" << dbg::endl;

    static_cast<void>(signal(SIGINT, intHandler));
    static_cast<void>(signal(SIGHUP, intHandler));
    static_cast<void>(signal(SIGQUIT, intHandler));
    static_cast<void>(signal(SIGTSTP, intHandler));
    static_cast<void>(signal(SIGTERM, intHandler));
    
    CBleSensorHandler sensors;
    sensors.scan();

    static_cast<void>(signal(SIGINT, SIG_DFL));
    static_cast<void>(signal(SIGHUP, SIG_DFL));
    static_cast<void>(signal(SIGQUIT, SIG_DFL));
    static_cast<void>(signal(SIGTSTP, SIG_DFL));
    static_cast<void>(signal(SIGTERM, SIG_DFL));

    dbg::sout << dbg::warn << "BLE sensors scanner stopped" << dbg::endl;

    return 0;
}
