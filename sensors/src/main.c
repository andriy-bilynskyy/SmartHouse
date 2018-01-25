#include <stdbool.h>
#include <stdint.h>
#include <ble_advdata.h>
#include <nordic_common.h>
#include <softdevice_handler.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <ble_gap.h>
#include <app_timer.h>
#include <nrf_gpio.h>
#include <nrf_drv_adc.h>
#include "sensor.h"


#define KEY_INPUT                       22 // Sensor key input P22
#define LED_OUTPUT                       6 // Sensor key input P06

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

static void ble_stack_init(void)
{
    nrf_clock_lf_cfg_t clock_lf_cfg = {.source = NRF_CLOCK_LF_SRC_XTAL, .rc_ctiv = 0, .rc_temp_ctiv  = 0, .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM};
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);
    ble_enable_params_t ble_enable_params;
    APP_ERROR_CHECK(softdevice_enable_get_default_config(0, 0, &ble_enable_params));
    CHECK_RAM_START_ADDR(0, 0);
    APP_ERROR_CHECK(softdevice_enable(&ble_enable_params));
}

static const char * sensor_name = NULL;

void sensor_set_name(const char * name)
{
    sensor_name = name;
}

static nrf_adc_value_t adc_vcc_values[10] = {0};

static void update_vcc_value(void)
{
    static bool inited = false;
    static size_t pointer = 0;

    static const nrf_drv_adc_channel_t cfg = {{{.resolution = NRF_ADC_CONFIG_RES_10BIT, 
                                                .input = ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling,
                                                .reference = NRF_ADC_CONFIG_REF_VBG,
                                                .ain = NRF_ADC_CONFIG_INPUT_DISABLED
                                              }}, NULL
                                             };
    nrf_adc_value_t value = 0;
    APP_ERROR_CHECK(nrf_drv_adc_sample_convert(&cfg, &value));
    if(!inited)
    {
        inited = true;
        for(size_t i = 0; i < sizeof(adc_vcc_values)/sizeof(nrf_adc_value_t); i++)
        {
            adc_vcc_values[i] = value;
        }
        pointer = 0;
    }
    else
    {
        adc_vcc_values[pointer++] = value;
        if(pointer >= sizeof(adc_vcc_values)/sizeof(nrf_adc_value_t))
        {
            pointer = 0;
        }
    }
}

static nrf_adc_value_t get_vcc_value(void)
{
    size_t sum = 0;
    for(size_t i = 0; i < sizeof(adc_vcc_values)/sizeof(nrf_adc_value_t); i++)
    {
        sum += adc_vcc_values[i];
    }
    return sum/(sizeof(adc_vcc_values)/sizeof(nrf_adc_value_t));
}

void sensor_advertise(const char * value)
{
    if(sensor_name && value)
    {
        static const char sensor_id[] = "sensor";
        size_t sensor_str_len = strlen(sensor_id) + strlen(sensor_name) + strlen(value) + 7;

        char sensor_str[sensor_str_len + 1];
        size_t cur_len = 0;
        strcpy(&sensor_str[cur_len], sensor_id);
        cur_len += strlen(sensor_str);
        sensor_str[cur_len] = ' ';
        strcpy(&sensor_str[cur_len + 1], sensor_name);
        cur_len = strlen(sensor_str);
        sensor_str[cur_len] = ' ';
        uint32_t vcc = 360u * get_vcc_value() / 1024u;
        sensor_str[cur_len + 1] = '0' + (vcc /100u) % 10u;
        sensor_str[cur_len + 2] = '.';
        sensor_str[cur_len + 3] = '0' + (vcc /10u) % 10u;
        sensor_str[cur_len + 4] = '0' + vcc % 10u;
        sensor_str[cur_len + 5] = ' ';
        strcpy(&sensor_str[cur_len + 6], value);

        NRF_LOG_INFO("Sensor data[%d]: %s\r\n",sensor_str_len,nrf_log_push(sensor_str));
        if(sensor_str_len > 24)
        {
            NRF_LOG_INFO("Advertisement data too long! Cut to 24 bytes.\r\n");
            sensor_str_len = 24;
        }

        ble_advdata_manuf_data_t manuf_specific_data;
        manuf_specific_data.data.p_data = (uint8_t *)sensor_str;
        manuf_specific_data.data.size   = sensor_str_len;
        manuf_specific_data.company_identifier = 0x0059;

        ble_advdata_t advdata;
        memset(&advdata, 0, sizeof(advdata));

        advdata.name_type             = BLE_ADVDATA_NO_NAME;
        advdata.flags                 = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
        advdata.p_manuf_specific_data = &manuf_specific_data;

        APP_ERROR_CHECK(ble_advdata_set(&advdata, NULL));
        //APP_ERROR_CHECK(sd_ble_gap_tx_power_set(4));
        ble_gap_adv_params_t adv_params;
        memset(&adv_params, 0, sizeof(adv_params));

        adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
        adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
        adv_params.fp          = BLE_GAP_ADV_FP_ANY;
        adv_params.interval    = MSEC_TO_UNITS(100, UNIT_0_625_MS);
        adv_params.timeout     = 0;

        APP_ERROR_CHECK(sd_ble_gap_adv_start(&adv_params));
    }
}

#if NRF_LOG_ENABLED == 1
static void reverse_array(uint8_t buf[], uint16_t len)
{
    for(uint8_t i = 0; i < len/2; i++)
    {
        uint8_t tmp = buf[len - i - 1];
        buf[len - i - 1] = buf[i];
        buf[i] = tmp;
    }
}
#endif

void sensor_led_on(void)
{
    nrf_gpio_cfg_output(LED_OUTPUT);
    nrf_gpio_pin_set(LED_OUTPUT);
}

void sensor_led_off(void)
{
    nrf_gpio_cfg_default(LED_OUTPUT);
    nrf_gpio_pin_clear(LED_OUTPUT);
}

//each 0.5 Second
static void on_wake_up(void * p_context)
{
    NRF_LOG_INFO("Wake up...\r\n");
    (void)sd_ble_gap_adv_stop();
    (void)p_context;
    static uint32_t vcc_cnt = 0;
    if(!vcc_cnt)
    {
        update_vcc_value();
        vcc_cnt = 10;
    }
    else
    {
        vcc_cnt --;
    }
    static bool force_update = true;
    if(nrf_gpio_pin_read(KEY_INPUT))
    {
        sensor_led_off();
        routine_sensor(force_update);
        force_update = false;
    }
    else
    {
        sensor_led_on();
        sensor_advertise("key");
        force_update = true;
    } 
}

int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    ble_stack_init();

    #if NRF_LOG_ENABLED == 1
    ble_gap_addr_t ble_addr;
    ret_code_t ret_code = sd_ble_gap_address_get(&ble_addr);
    if(ret_code == NRF_SUCCESS)
    {
        reverse_array(ble_addr.addr, BLE_GAP_ADDR_LEN);
        NRF_LOG_HEXDUMP_INFO(ble_addr.addr, BLE_GAP_ADDR_LEN);
    }
    else
    {
        NRF_LOG_INFO("Get address error!\r\n");
    }
    #endif

    start_sensor();

    nrf_gpio_cfg_input(KEY_INPUT, NRF_GPIO_PIN_PULLUP);
    
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
    APP_ERROR_CHECK(nrf_drv_adc_init(&config, NULL));

    APP_TIMER_INIT(0, 4, false);
    APP_TIMER_DEF(apptimer_id);
    APP_ERROR_CHECK(app_timer_create(&apptimer_id, APP_TIMER_MODE_REPEATED, on_wake_up));
    APP_ERROR_CHECK(app_timer_start(apptimer_id, APP_TIMER_TICKS(500, 0), NULL));
    NRF_LOG_INFO("Sensor init done!\r\n");

    for (;;)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            APP_ERROR_CHECK(sd_app_evt_wait());
        }
    }
}
