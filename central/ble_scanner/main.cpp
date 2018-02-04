#include <CBleScanner.h>
#include <CDBHandler.h>
#include <safeout.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>


struct protected_db_t
{
    protected_db_t(CDBHandler & db, pthread_mutex_t & db_protector) : m_db(db), m_db_protector(db_protector) {}
    CDBHandler       & m_db;
    pthread_mutex_t  & m_db_protector;
};


class CBLEhandler : public CBleSensorScanner
{
public:
    explicit CBLEhandler(protected_db_t & protected_db) : m_protected_db(protected_db) {}

private:
    protected_db_t & m_protected_db;

    virtual void on_sensor_advertising(std::string addr, std::string sen_type, std::string sen_voltage, std::string sen_value)
    {
        dbg::sout << dbg::info << addr << " : " << sen_type << " : " << sen_voltage << " : "  << sen_value << dbg::endl;

        if(pthread_mutex_lock(&m_protected_db.m_db_protector) == EOWNERDEAD)
        {
            (void)pthread_mutex_consistent(&m_protected_db.m_db_protector);
        }
        m_protected_db.m_db.add_sensor(addr, sen_type, sen_voltage, sen_value);
        (void)pthread_mutex_unlock(&m_protected_db.m_db_protector);
    }
};


void intHandler(int dummy)
{
    static_cast<void>(dummy);
    dbg::sout << dbg::info << "Terminating..." << dbg::endl;
    CBleSensorScanner::m_terminate_scan = true;
}

static void * db_sensor_insert(void *arg)
{
    dbg::sout.setDbgLevel(dbg::DBG_INFO);
    dbg::sout << dbg::dbg << "Thread started" << dbg::endl;
    CBLEhandler ble(*reinterpret_cast<protected_db_t *>(arg));
    ble.scan();
    dbg::sout << dbg::dbg << "Thread stopped" << dbg::endl;
    (void)kill(getpid(), SIGINT);
    return NULL;
}



int main(void)
{
    dbg::sout.setDbgLevel(dbg::DBG_DEBUG); // (dbg::DBG_INFO);
    dbg::sout << dbg::warn << "BLE sensors scanner started" << dbg::endl;

    CDBHandler db;
    db.add_log("BLUETOOTH", "BLE sensors scanner started");

    static_cast<void>(signal(SIGINT, intHandler));
    static_cast<void>(signal(SIGHUP, intHandler));
    static_cast<void>(signal(SIGQUIT, intHandler));
    static_cast<void>(signal(SIGTSTP, intHandler));
    static_cast<void>(signal(SIGTERM, intHandler));

    pthread_mutexattr_t attr;
    pthread_mutex_t     protector;
    (void)pthread_mutexattr_init(&attr);
    (void)pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
    (void)pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    (void)pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
    (void)pthread_mutex_init(&protector, &attr);
    (void)pthread_mutexattr_destroy(&attr);

    protected_db_t p_db(db, protector);
    pthread_t      thread;
    if(pthread_create(&thread, NULL, db_sensor_insert, &p_db))
    {
        dbg::sout << dbg::err << "Thread creation failed" << dbg::endl;
    }

    while(!CBleSensorScanner::m_terminate_scan)
    {
        if(pthread_mutex_lock(&p_db.m_db_protector) == EOWNERDEAD)
        {
            (void)pthread_mutex_consistent(&p_db.m_db_protector);
        }
        p_db.m_db.rm_obsolete_sensors();
        (void)pthread_mutex_unlock(&p_db.m_db_protector);
        
        for(uint32_t i = 0; i < 1000 && !CBleSensorScanner::m_terminate_scan; i++)
        {
            usleep(10000);
        }

    }

    if(pthread_join(thread, NULL))
    {
        dbg::sout << dbg::err << "Thread join failed" << dbg::endl;
    }

    (void)pthread_mutex_destroy(&protector);

    static_cast<void>(signal(SIGINT, SIG_DFL));
    static_cast<void>(signal(SIGHUP, SIG_DFL));
    static_cast<void>(signal(SIGQUIT, SIG_DFL));
    static_cast<void>(signal(SIGTSTP, SIG_DFL));
    static_cast<void>(signal(SIGTERM, SIG_DFL));

    db.add_log("BLUETOOTH", "BLE sensors scanner stopped");
    dbg::sout << dbg::warn << "BLE sensors scanner stopped" << dbg::endl;

    return 0;
}
