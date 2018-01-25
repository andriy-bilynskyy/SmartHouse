#ifndef CBLESCANNER_H_
#define CBLESCANNER_H_

#include <stdint.h>
#include <string>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


class CBleSensorScanner
{
public:
    CBleSensorScanner();
    virtual ~CBleSensorScanner();
    void scan();

    static volatile bool    m_terminate_scan;

private:
    int                     m_handle;
    struct hci_filter       m_original_filter;
    static const uint8_t    m_sensor_tag[];
    static const uint8_t    EIR_MANUFACTURE_SPECIFIC;

    bool start_hci_scan();
    void stop_hci_scan() const;
    static std::string process_adv_data(uint8_t data[], uint32_t len);

    virtual void on_sensor_advertising(std::string addr, std::string sen_type, std::string sen_voltage, std::string sen_value) = 0;
};

#endif
