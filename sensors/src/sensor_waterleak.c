#include "sensor.h"
#include <nrf_log.h>
#include <app_error.h>
#include <nrf_drv_adc.h>


#define ADC_INPUT NRF_ADC_CONFIG_INPUT_5 // Sensor ADC input. 5 -> P04
#define ADC_THRESHOLD 400

void start_sensor(void)
{
    sensor_set_name("w_leak");
    NRF_LOG_INFO("Sensor started!\r\n");
}


static nrf_adc_value_t adc_values[5] = {0};

static void update_adc_value(void)
{
    static bool inited = false;
    static size_t pointer = 0;

    static const nrf_drv_adc_channel_t cfg = {{{.resolution = NRF_ADC_CONFIG_RES_10BIT, 
                                                .input = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD,
                                                .reference = NRF_ADC_CONFIG_REF_VBG,
                                                .ain = ADC_INPUT
                                              }}, NULL
                                             };
    nrf_adc_value_t value = 0;
    APP_ERROR_CHECK(nrf_drv_adc_sample_convert(&cfg, &value));
    if(!inited)
    {
        inited = true;
        for(size_t i = 0; i < sizeof(adc_values)/sizeof(nrf_adc_value_t); i++)
        {
            adc_values[i] = value;
        }
        pointer = 0;
    }
    else
    {
        adc_values[pointer++] = value;
        if(pointer >= sizeof(adc_values)/sizeof(nrf_adc_value_t))
        {
            pointer = 0;
        }
    }
}

static nrf_adc_value_t get_vcc_value(void)
{
    size_t sum = 0;
    for(size_t i = 0; i < sizeof(adc_values)/sizeof(nrf_adc_value_t); i++)
    {
        sum += adc_values[i];
    }
    return sum/(sizeof(adc_values)/sizeof(nrf_adc_value_t));
}


void routine_sensor(bool force_update)
{
    static uint32_t timer_adv = 0;
    static bool sensor_state = true;
    bool state_updated = false;

    update_adc_value();
    nrf_adc_value_t value = get_vcc_value();

    if(value < ADC_THRESHOLD && sensor_state)
    {
        sensor_state = false;
        state_updated = true;
    }
    if(value >= ADC_THRESHOLD && !sensor_state)
    {
        sensor_state = true;
        state_updated = true;
    }

    if(!timer_adv || state_updated || force_update)
    {
        if(sensor_state)
        {
            timer_adv = 2 * 60 * 10;
            sensor_advertise("dry");
        }
        else
        {
            timer_adv = 2 * 10;
            sensor_advertise("wet");
        }
    }
    else
    {
        timer_adv --;
    }
}
