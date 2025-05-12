#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "libs/bootsel.h"
#include "libs/leds.h"
#include "libs/temperature_sensor.h"
#include "libs/wifi.h"
#include "libs/display.h"
#include "libs/webserver.h"

// Credenciais WIFI - Tome cuidado se publicar no github!
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

void init() {
    stdio_init_all();
    bootsel_init();
    temperature_sensor_init();
    leds_init();
    wifi_init();
    display_init();
}

void web_request_handler(char *route) {
    if (strstr(route, "blue_on") != NULL) {
        leds_set_blue(true);
        return;
    }
    if (strstr(route, "blue_off") != NULL) {
        leds_set_blue(false);
        return;
    }
    if (strstr(route, "green_on") != NULL) {
        leds_set_green(true);
        return;
    }
    if (strstr(route, "green_off") != NULL) {
        leds_set_green(false);
        return;
    }
    if (strstr(route, "red_on") != NULL) {
        leds_set_red(true);
        return;
    }
    if (strstr(route, "red_off") != NULL) {
        leds_set_red(false);
        return;
    }
    if (strstr(route, "on") != NULL) {
        wifi_set_led(true);
        return;
    }
    if (strstr(route, "off") != NULL) {
        wifi_set_led(false);
        return;
    }
};

int main() {
    init();
    if (!wifi_is_init_success()) {
        display_set_second_line_message("Falha ao inicializar Wi-Fi");
        sleep_ms(1000);
        return -1;
    }
    display_set_second_line_message("Conectando ao Wi-Fi...");
    if (wifi_connect_to(WIFI_SSID, WIFI_PASSWORD)) {
        display_set_second_line_message("Falha ao conectar ao Wi-Fi");
        sleep_ms(1000);
        return -1;
    }
    display_set_second_line_message("Conectado ao Wi-Fi");
    sleep_ms(1000);
    display_set_first_line_message(wifi_ip());
    if (!webserver_create(80, web_request_handler, temperature_sensor_read)){
        display_set_second_line_message("Falha ao criar servidor web");
    }
    display_set_second_line_message("Serv web criado porta 80");
    wifi_keep_active();
    wifi_deinit();
    return 0;
}