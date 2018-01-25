#include "CBleScanner.h"
#include <unistd.h>
#include "../safeout/safeout.h"
#include <errno.h>


volatile bool CBleSensorScanner::m_terminate_scan = false;

const uint8_t CBleSensorScanner::m_sensor_tag[]   = "sensor";
const uint8_t CBleSensorScanner::EIR_MANUFACTURE_SPECIFIC = 0xFF;


CBleSensorScanner::CBleSensorScanner() : m_handle(-1), m_original_filter()
{
    m_handle = hci_open_dev(hci_get_route(NULL));
    if(m_handle < 0)
    {
        dbg::sout << dbg::err << "Could not open device: " << strerror(errno) << dbg::endl;
    }
}

CBleSensorScanner::~CBleSensorScanner()
{
    if(m_handle >= 0)
    {
        if(hci_close_dev(m_handle) < 0)
        {
            dbg::sout << dbg::err << "Could not close device: " << strerror(errno) << dbg::endl;
        }
    }
}

void CBleSensorScanner::scan()
{
    if(start_hci_scan())
    {
        dbg::sout << dbg::info << "Scanning..." << dbg::endl;
        while(!m_terminate_scan)
        {
            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 10000;
            fd_set set;
            FD_ZERO(&set);
            FD_SET(m_handle, &set);
            if(select(m_handle + 1, &set, NULL, NULL, &timeout) > 0)
            {
                uint8_t buf[HCI_MAX_EVENT_SIZE];
                int len = read(m_handle, buf, sizeof(buf));
                if (len >= static_cast<int>(5 + sizeof(le_advertising_info)))
                {
                    if(buf[3] == EVT_LE_ADVERTISING_REPORT)
                    {
                        le_advertising_info *info = reinterpret_cast<le_advertising_info *>(&buf[5]);
                        if(info->length == len - 6 - sizeof(le_advertising_info))
                        {
                            std::string sensor_str = process_adv_data(info->data, info->length);
                            if(sensor_str != "")
                            {
                                 
                                char addr[18];
                                ba2str(&info->bdaddr, addr);
                                std::string type, voltage, value;
                                size_t pos = sensor_str.find(' ');
                                if(pos != std::string::npos)
                                {
                                    type = sensor_str.substr(0, pos);
                                    sensor_str = sensor_str.substr(pos + 1, std::string::npos);
                                }
                                pos = sensor_str.find(' ');
                                if(pos != std::string::npos)
                                {
                                    voltage = sensor_str.substr(0, pos);
                                    value = sensor_str.substr(pos + 1, std::string::npos);
                                }
                                on_sensor_advertising(addr, type, voltage, value);
                            }
                        }
                    }
                }
            }
        }
        stop_hci_scan();
    }
    else
    {
        dbg::sout << dbg::err << "Scanning not started" << dbg::endl;
    }
}

bool CBleSensorScanner::start_hci_scan()
{
    if(m_handle < 0)
    {
        dbg::sout << dbg::err << "Device is not opened" << dbg::endl;
        return false;
    }
    if(hci_le_set_scan_parameters(m_handle, 0, 0x10, 0x10, 0x00, 0x00, 1000) < 0)
    {
        dbg::sout << dbg::err << "Failed to set scan parameters: " << strerror(errno) << dbg::endl;
        return false;
    }
    if(hci_le_set_scan_enable(m_handle, 0x01, 0, 1000) < 0)
    {
        dbg::sout << dbg::err << "Failed to enable scan: " << strerror(errno) << dbg::endl;
        return false;
    }
    socklen_t olen = sizeof(struct hci_filter);
    if(getsockopt(m_handle, SOL_HCI, HCI_FILTER, &m_original_filter, &olen) < 0)
    {
        dbg::sout << dbg::err << "Could not get socket options: " << strerror(errno) << dbg::endl;
        if(hci_le_set_scan_enable(m_handle, 0x00, 1, 1000) < 0)
        {
            dbg::sout << dbg::err << "Disable scan failed" << strerror(errno) << dbg::endl;
        }
        return false;
    }
    struct hci_filter new_filter;
    hci_filter_clear(&new_filter);
    hci_filter_set_ptype(HCI_EVENT_PKT, &new_filter);
    hci_filter_set_event(EVT_LE_META_EVENT, &new_filter);
    if(setsockopt(m_handle, SOL_HCI, HCI_FILTER, &new_filter, sizeof(struct hci_filter)) < 0)
    {
        dbg::sout << dbg::err << "Could not set socket options: " << strerror(errno) << dbg::endl;
        if(hci_le_set_scan_enable(m_handle, 0x00, 1, 1000) < 0)
        {
            dbg::sout << dbg::err << "Disable scan failed" << strerror(errno) << dbg::endl;
        }
        return false;
    }
    return true;
}


void CBleSensorScanner::stop_hci_scan() const
{
    if(m_handle >= 0)
    {
        if(setsockopt(m_handle, SOL_HCI, HCI_FILTER, &m_original_filter, sizeof(struct hci_filter)) < 0)
        {
            dbg::sout << dbg::err << "Could not set original socket options: " << strerror(errno) << dbg::endl;
        }
        if(hci_le_set_scan_enable(m_handle, 0x00, 1, 1000) < 0)
        {
            dbg::sout << dbg::err << "Disable scan failed" << strerror(errno) << dbg::endl;
        }
    }
}

std::string CBleSensorScanner::process_adv_data(uint8_t data[], uint32_t len)
{
    //dbg::sout << dbg::warn << dbg::dbgout::bufToHEXstr(data, len) << dbg::endl;
    std::string result = "";
    if(len)
    {
        if(len > data[0])
        {
            if(data[0] >= 4 + sizeof(m_sensor_tag) - 1)
            {
                if(data[1] == EIR_MANUFACTURE_SPECIFIC)
                {
                    if(std::equal(&m_sensor_tag[0], &m_sensor_tag[sizeof(m_sensor_tag) - 1], &data[4]))
                    {
                        result = std::string(reinterpret_cast<char *>(&data[4 + sizeof(m_sensor_tag)]), data[0] - (4 + sizeof(m_sensor_tag) - 1));
                    }
                }
            }
            if(result == "")
            {
                result = process_adv_data(&data[data[0] + 1], len - data[0] - 1);
            }
        }
    }
    return result;
}

