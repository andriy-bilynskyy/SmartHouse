#ifndef CDBHANDLER_H_
#define CDBHANDLER_H_

#include <stdint.h>
#include <mysql/mysql.h>
#include <string>


class CDBHandler
{
public:
    CDBHandler();
    ~CDBHandler();
    void add_sensor(std::string addr, std::string sen_type, std::string sen_voltage, std::string sen_value);
    void rm_obsolete_sensors();
    void add_log(std::string unit, std::string message);
    bool check_water_fault(bool logFault);
    bool get_water_valve();
    void set_water_valve(bool state);
    bool need_water_scheduled_turn();

private:

    MYSQL                         * m_connection;
    bool                            m_db_connected;

    static bool                     m_isLibInited;
    static uint32_t                 m_instanceCnt;

    static const char               m_DB_host[];
    static const char               m_DB_user[];
    static const char               m_DB_password[];
    static const char               m_DB_name[];
};

#endif
