#include "leds.h"
#include <stdbool.h>
#include "pico/stdlib.h"

#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12
#define RED_LED_PIN 13

static void led_init(int led_pin) {
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_put(led_pin, false);
}

void leds_init() {
    led_init(GREEN_LED_PIN);
    led_init(BLUE_LED_PIN);
    led_init(RED_LED_PIN);
}

void leds_set_red(bool status) {
    gpio_put(RED_LED_PIN, status);
}

void leds_set_green(bool status) {
    gpio_put(GREEN_LED_PIN, status);
}

void leds_set_blue(bool status) {
    gpio_put(BLUE_LED_PIN, status);
}