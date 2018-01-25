#ifndef SENSOR_H_
#define SENSOR_H_

#include <stdbool.h>

extern void sensor_set_name(const char * name);
extern void sensor_advertise(const char * value);

extern void sensor_led_on(void);
extern void sensor_led_off(void);

void start_sensor(void);
void routine_sensor(bool force_update);


#endif