#include "sensor.h"
#include <nrf_log.h>
#include <app_error.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>

#define SENSOR_PIN                       29 // DS18B20 data pin
#define POWER_PIN                        28 // DS18B20 power enable pin


void start_sensor(void)
{
    sensor_set_name("temp");
    NRF_LOG_INFO("Sensor started!\r\n");
}

static bool powerState = false;             // power off

static inline void power_on(void)
{
    nrf_gpio_cfg_output(POWER_PIN);
    nrf_gpio_pin_clear(POWER_PIN);
    nrf_delay_us(400);
    powerState = true;
}

static inline void power_off(void)
{
    nrf_gpio_cfg_default(POWER_PIN);
    nrf_gpio_pin_clear(POWER_PIN);
    powerState = false;
}

static inline void therm_input_mode(void)
{
    nrf_gpio_cfg_input(SENSOR_PIN, NRF_GPIO_PIN_NOPULL);
}

static inline void therm_output_mode(void)
{
    nrf_gpio_cfg_output(SENSOR_PIN);
}

static inline void therm_low(void)
{
    nrf_gpio_pin_clear(SENSOR_PIN);
}

static inline void therm_high(void)
{
    nrf_gpio_pin_set(SENSOR_PIN);
}

static inline uint8_t therm_read(void)
{
    return nrf_gpio_pin_read(SENSOR_PIN);
}

static inline bool therm_reset(void)
{
    bool result;
    therm_low();
    therm_output_mode();
    nrf_delay_us(480);
    therm_input_mode();
    nrf_delay_us(60);
    result = therm_read();
    nrf_delay_us(420);
    return !result;
}
 
static inline void therm_write_bit(uint8_t bit)
{
    therm_low();
    therm_output_mode();
    nrf_delay_us(1);
    if(bit) therm_input_mode();
    nrf_delay_us(60);
    therm_input_mode();
}
 
static inline uint8_t therm_read_bit(void)
{
    uint8_t bit = 0;
    therm_low();
    therm_output_mode();
    nrf_delay_us(1);
    therm_input_mode();
    nrf_delay_us(14);
    if(therm_read()) bit = 1;
    nrf_delay_us(45);
    return bit;
}
 
static uint8_t therm_read_byte(void)
{
    uint8_t i = 8, n = 0;
    while(i--)
    {
        n >>= 1;
        n |= (therm_read_bit() << 7);
    }
    return n;
}
 
static void therm_write_byte(uint8_t byte)
{
    uint8_t i = 8;
    while(i--)
    {
        therm_write_bit(byte & 1);
        byte >>= 1;
    }
}

static bool therm_start_convert(void)
{
    bool conv_started = false;
 
    if(therm_reset())
    {
        therm_write_byte(0xCC);
        therm_write_byte(0x44);
        conv_started = true;
    }
 
    return conv_started;
}

static inline bool term_ready(void)
{
    return therm_read_bit();
}

static bool therm_read_temperature(int16_t * temperature)
{
    bool data_avail = false;
    uint8_t temp[2] = {0};
    
    if(therm_reset())
    {
        therm_write_byte(0xCC);
        therm_write_byte(0xBE);
 
        temp[0] = therm_read_byte();
        temp[1] = therm_read_byte();
        if(therm_reset())
        {
            *temperature =  ((((uint16_t)temp[1] << 8) | temp[0]) * 10) / 16; 
            data_avail = true;
        }
    }
 
    return data_avail;
}

static char * temperature_str(int16_t t, char * str)
{
    uint8_t neg = 0, len = 0;
    if(t < 0)
    {
        neg = 1;
        t = -t;
        str[0] = '-';
        len = 1;

    }
    uint8_t rem = t % 10;
    t /= 10;
    do
    {
        uint8_t i = 0;
        for(i = len; i > neg; i--)
        {
            str[i] = str[i - 1];
        }
        str[neg] = '0' + t % 10;
        len ++;
        t /= 10;
    }while(t > 0);
    str[len] = '.';
    str[len + 1] = '0' + rem;
    str[len + 2] = '\0';
    return str;
}

void routine_sensor(bool force_update)
{
    static uint32_t timer_adv = 0;

    if(powerState)
    {
        if(term_ready())
        {
            int16_t temperature = 0;
            if(therm_read_temperature(&temperature))
            {
                char buf[10];
                sensor_advertise(temperature_str(temperature, buf));
            }
            else
            {
                sensor_advertise("error");
            }
            power_off();    
        }
        else if(timer_adv < 2 * 60 * 10 - 2)
        {
            timer_adv --;
            sensor_advertise("error");
            power_off();
        }
    }
    else if(!timer_adv || force_update)
    {
        timer_adv = 2 * 60 * 10;
        NRF_LOG_INFO("Timer %u\r\n", timer_adv);

        power_on();
        if(!therm_start_convert())
        {
            sensor_advertise("error");
            power_off();
        }
    }
    else
    {
        timer_adv --;
    }
}