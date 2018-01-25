#include "CDBHandler.h"
#include "../safeout/safeout.h"
#include "credentials.h"


bool CDBHandler::m_isLibInited   = false;
uint32_t CDBHandler::m_instanceCnt   = 0;

const char CDBHandler::m_DB_host[]             = "localhost";
const char CDBHandler::m_DB_user[]             = DB_CREDENTIAL_USERNAME;
const char CDBHandler::m_DB_password[]         = DB_CREDENTIAL_PASSWORD;
const char CDBHandler::m_DB_name[]             = "smart_house";


CDBHandler::CDBHandler() : m_connection(NULL),
                           m_db_connected(false)
{
    //critical section
    m_instanceCnt ++;
    if(!m_isLibInited)
    {
        if(!mysql_library_init(0, NULL, NULL))
        {
            m_isLibInited = true;
        }
        else
        {
            dbg::sout << dbg::err << "mysql_library_init error" << dbg::endl;
        }
    }
    //critical section
    if(m_isLibInited)
    {
        m_connection = mysql_init(NULL);
        if(m_connection)
        {
            my_bool reconnect = true;
            if(!mysql_options(m_connection, MYSQL_OPT_RECONNECT, &reconnect))
            {
                if(mysql_real_connect(m_connection, m_DB_host, m_DB_user, m_DB_password, m_DB_name, 0, NULL, 0))
                {
                    m_db_connected = true;
                    dbg::sout << dbg::info << "database connected" << dbg::endl;
                }
                else
                {
                    dbg::sout << dbg::err << "mysql_real_connect error: " << mysql_error(m_connection) << dbg::endl;
                }
            }
            else
            {
                dbg::sout << dbg::err << "mysql_options error" << dbg::endl;
            }
        }
        else
        {
            dbg::sout << dbg::err << "mysql_init error" << dbg::endl;
        }
    }
}

CDBHandler::~CDBHandler()
{
    if(m_db_connected)
    {
        mysql_close(m_connection);
        dbg::sout << dbg::info << "database disconnected" << dbg::endl;
    }
    //critical section
    m_instanceCnt --;
    if(!m_instanceCnt && m_isLibInited)
    {
        mysql_library_end();
        m_isLibInited = false;
    }
    //critical section
}

void CDBHandler::add_sensor(std::string addr, std::string sen_type, std::string sen_voltage, std::string sen_value)
{
    if(m_db_connected)
    {
        std::string add_sensor_query = "INSERT INTO on_line_sensors (address, type, voltage, value) VALUES ('" +
                                        addr + "', '" +
                                        sen_type + "', '" +
                                        sen_voltage + "', '" +
                                        sen_value + "')" +
                                        " ON DUPLICATE KEY UPDATE type='" +
                                        sen_type + "', voltage='" +
                                        sen_voltage + "', value='" +
                                        sen_value + "', last_seen=NOW()";

        dbg::sout << dbg::info << add_sensor_query << dbg::endl;

        if(mysql_query(m_connection, add_sensor_query.c_str()))
        {
            dbg::sout << dbg::err << "mysql_query add sensor query error: " << mysql_error(m_connection) << dbg::endl;
        }
    }
    else
    {
        dbg::sout << dbg::err << "mysql not connected" << dbg::endl;
    }
}

void CDBHandler::rm_obsolete_sensors()
{
    if(m_db_connected)
    {
        std::string rm_sensors_query = "DELETE FROM on_line_sensors WHERE TIMEDIFF(NOW(), last_seen)>'24:00:00' or last_seen IS NULL";

        dbg::sout << dbg::info << rm_sensors_query << dbg::endl;

        if(mysql_query(m_connection, rm_sensors_query.c_str()))
        {
            dbg::sout << dbg::err << "mysql_query rm sensors query error: " << mysql_error(m_connection) << dbg::endl;
        }
    }
    else
    {
        dbg::sout << dbg::err << "mysql not connected" << dbg::endl;
    }   
}

void CDBHandler::add_log(std::string message)
{
    if(m_db_connected)
    {
        std::string add_log_query = "INSERT INTO data_log (unit, message) VALUES ('BLUETOOTH', '" + message + "')";
        
        dbg::sout << dbg::info << add_log_query << dbg::endl;
        
        if(mysql_query(m_connection, add_log_query.c_str()))
        {
            dbg::sout << dbg::err << "mysql_query add log error: " << mysql_error(m_connection) << dbg::endl;
        }
    }
    else
    {
        dbg::sout << dbg::err << "mysql not connected" << dbg::endl;
    }
}

bool CDBHandler::check_water_fault(bool logFault)
{
    bool result = false;
    if(m_db_connected)
    {
        std::string get_fault_query = "SELECT subscribed_wet_sensors.address FROM subscribed_wet_sensors "
                                      "LEFT JOIN on_line_sensors ON on_line_sensors.address=subscribed_wet_sensors.address "
                                      "WHERE on_line_sensors.value IS NULL or on_line_sensors.value='wet'";
        
        dbg::sout << dbg::info << get_fault_query << dbg::endl;

        if(!mysql_query(m_connection, get_fault_query.c_str()))
        {
            MYSQL_RES *res = mysql_store_result(m_connection);
            if(res)
            {
                my_ulonglong rows = mysql_num_rows(res);
                if(!rows)
                {
                    result = true;
                }
                else
                {
                    if(logFault)
                    {
                        for(my_ulonglong i = 0; i < rows; i++)
                        {
                            MYSQL_ROW row = mysql_fetch_row(res);
                            dbg::sout << dbg::warn << "failed water leak sensor: " << row[0] << dbg::endl;
                            add_log(std::string("failed water leak sensor: ") + row[0]);
                        }
                    }
                }
                mysql_free_result(res);
            }
            else
            {
                dbg::sout << dbg::err << "mysql_store_result get fault water leak sensors error: " << mysql_error(m_connection) << dbg::endl;
            }
        }
        else
        {
            dbg::sout << dbg::err << "mysql_query get fault water leak sensors error: " << mysql_error(m_connection) << dbg::endl;
        }
    }
    else
    {
        dbg::sout << dbg::err << "mysql not connected" << dbg::endl;
    }
    return result;
}

bool CDBHandler::get_water_valve()
{
    bool result = false;
    if(m_db_connected)
    {
        std::string get_valve_query = "SELECT state FROM valves WHERE name='main water tap'";

        dbg::sout << dbg::info << get_valve_query << dbg::endl;

        if(!mysql_query(m_connection, get_valve_query.c_str()))
        {
            MYSQL_RES *res = mysql_store_result(m_connection);
            if(res)
            {
                if(mysql_num_rows(res) == 1)
                {
                    MYSQL_ROW row = mysql_fetch_row(res);
                    if(std::string(row[0]) == "1")
                    {
                        result = true;
                    }
                }
                mysql_free_result(res);
            }
            else
            {
                dbg::sout << dbg::err << "mysql_store_result get valve error: " << mysql_error(m_connection) << dbg::endl;
            }
        }
        else
        {
            dbg::sout << dbg::err << "mysql_query get valve error: " << mysql_error(m_connection) << dbg::endl;
        }

    }
    else
    {
        dbg::sout << dbg::err << "mysql not connected" << dbg::endl;
    }
    return result;
}

void CDBHandler::set_water_valve(bool state)
{
    if(m_db_connected)
    {
        if(get_water_valve() != state)
        {
            std::string set_valve_query = state?"UPDATE valves SET state=1 WHERE name='main water tap'":
                                                "UPDATE valves SET state=0 WHERE name='main water tap'";

            dbg::sout << dbg::info << set_valve_query << dbg::endl;

            if(mysql_query(m_connection, set_valve_query.c_str()))
            {
                dbg::sout << dbg::err << "mysql_query set valve error: " << mysql_error(m_connection) << dbg::endl;
            }
        }
    }
    else
    {
        dbg::sout << dbg::err << "mysql not connected" << dbg::endl;
    }    
}

bool CDBHandler::need_water_scheduled_turn()
{
    bool result = false;
    if(m_db_connected)
    {
        std::string need_schedule_query = "SELECT (HOUR(NOW()) = 4) and (DATEDIFF(NOW(), (SELECT op_time FROM valves WHERE name='main water tap')) > 30)";

        dbg::sout << dbg::info << need_schedule_query << dbg::endl;

        if(!mysql_query(m_connection, need_schedule_query.c_str()))
        {
            MYSQL_RES *res = mysql_store_result(m_connection);
            if(res)
            {
                if(mysql_num_rows(res) == 1)
                {
                    MYSQL_ROW row = mysql_fetch_row(res);
                    if(std::string(row[0]) == "1")
                    {
                        result = true;
                    }
                }
                else
                {
                    dbg::sout << dbg::err << "water_valve table is empty" << dbg::endl;
                }
                mysql_free_result(res);
            }
            else
            {
                dbg::sout << dbg::err << "mysql_store_result scheduled service error: " << mysql_error(m_connection) << dbg::endl;
            }
        }
        else
        {
            dbg::sout << dbg::err << "mysql_query scheduled service error: " << mysql_error(m_connection) << dbg::endl;
        }
    }
    else
    {
        dbg::sout << dbg::err << "mysql not connected" << dbg::endl;
    }
    return result;    
}
