#include "temperature_sensor.h"
#include "hardware/adc.h"

#define ADC_SELECTOR 4
#define MAX_VOLTAGE 3.3f
#define ADC_MAX_VALUE 4096.0f

void temperature_sensor_init() {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_SELECTOR);
}

static float to_celsius(float raw_value) {
    float voltage = MAX_VOLTAGE * raw_value/ADC_MAX_VALUE;
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

float temperature_sensor_read() {
    unsigned int raw_value = adc_read();
    return to_celsius(raw_value);
}