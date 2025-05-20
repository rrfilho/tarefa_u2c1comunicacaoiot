#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "libs/buttons.h"
#include "libs/leds.h"
#include "libs/wifi.h"
#include "libs/display.h"
#include "libs/led_matrix.h"
#include "libs/buzzer.h"
#include "libs/webserver.h"
#include "html.h"

// Credenciais WIFI - Tome cuidado ao publicar no github!
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

bool entrance_light = false;
bool room_light = false;
bool kitchen_light = false;
bool bedroom_light = false;
unsigned int temperature = 20;
bool alarm = false;

void alarm_turn_on() {
    alarm = true;
    buzzer_set(true);
}

void init() {
    stdio_init_all();
    buttons_init(alarm_turn_on);
    leds_init();
    led_matrix_init();
    buzzer_init();
    wifi_init();
    display_init();
}

void web_request_handler(char *route) {
    if (strstr(route, "entrance_light") != NULL) {
        entrance_light = !entrance_light;
        led_matrix_lights(entrance_light, room_light, kitchen_light, bedroom_light);
        return;
    }
    if (strstr(route, "bedroom_light") != NULL) {
        bedroom_light = !bedroom_light;
        led_matrix_lights(entrance_light, room_light, kitchen_light, bedroom_light);
        return;
    }
    if (strstr(route, "room_light") != NULL) {
        room_light = !room_light;
        led_matrix_lights(entrance_light, room_light, kitchen_light, bedroom_light);
        return;
    }
    if (strstr(route, "kitchen_light") != NULL) {
        kitchen_light = !kitchen_light;
        led_matrix_lights(entrance_light, room_light, kitchen_light, bedroom_light);
        return;
    }
    if (strstr(route, "temp_inc") != NULL) {
        temperature = temperature + 1 > 35 ? temperature : temperature + 1;
        leds_set_red(temperature);
        return;
    }
    if (strstr(route, "temp_dec") != NULL) {
        temperature = temperature - 1 < 15 ? temperature : temperature - 1;
        leds_set_red(temperature);
        return;
    }
    if (strstr(route, "alarm_off") != NULL) {
        alarm = false;
        buzzer_set(false);
        return;
    }
};

char* get_light_class(bool status) {
    return status ? "on" : "off";
}

char* get_light_status(bool status) {
    return status ? "Acesa" : "Apagada";
} 

char* get_light_action(bool status) {
    return status ? "Apagar" : "Acender";
}

char* get_alarm_status() {
    return alarm ? "Alarme ativo" : "Sem alarme";
}

char* get_alarm_class() {
    return alarm ? "off" : "on";
}

char* get_alarm_button() {
    return alarm ? "" : "d-none";
}

char* content() {
    static char content[3110];
    snprintf(
        content, 3110, html, 
        get_light_class(entrance_light),
        get_light_status(entrance_light),
        get_light_action(entrance_light),
        get_light_class(room_light),
        get_light_status(room_light),
        get_light_action(room_light),
        get_light_class(kitchen_light),
        get_light_status(kitchen_light),
        get_light_action(kitchen_light),
        get_light_class(bedroom_light),
        get_light_status(bedroom_light),
        get_light_action(bedroom_light),
        temperature,
        get_alarm_class(),
        get_alarm_status(),
        get_alarm_button()
    );
    return content;
}

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
    if (!webserver_create(80, web_request_handler, content)){
        display_set_second_line_message("Falha ao criar servidor web");
    }
    display_set_second_line_message("Serv web criado porta 80");
    wifi_keep_active();
    wifi_deinit();
    return 0;
}