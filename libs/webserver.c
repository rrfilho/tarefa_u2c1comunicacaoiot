#include "webserver.h"
#include <string.h>
#include <stdlib.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define END_HEADER_REQUEST "1.1"

static const char* html = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<title> Embarcatech - LED Control </title>\n"
   "<style>\n"
    "body { background-color: #b5e5fb; font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
    "h1 { font-size: 50px; margin-bottom: 25px; }\n"
    "button { background-color: LightGray; font-size: 30px; margin: 8px; padding: 14px 44px; border-radius: 10px; }\n"
    ".temp { font-size: 40px; margin-top: 25px; color: #333; }\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<h1>Embarcatech: LED Control</h1>\n"
    "<form action=\"./blue_on\"><button>Ligar Azul</button></form>\n"
    "<form action=\"./blue_off\"><button>Desligar Azul</button></form>\n"
    "<form action=\"./green_on\"><button>Ligar Verde</button></form>\n"
    "<form action=\"./green_off\"><button>Desligar Verde</button></form>\n"
    "<form action=\"./red_on\"><button>Ligar Vermelho</button></form>\n"
    "<form action=\"./red_off\"><button>Desligar Vermelho</button></form>\n"
    "<p class=\"temp\">Temperatura Interna: %.2f &deg;C</p>\n"
    "</body>\n"
    "</html>";

static struct tcp_pcb *_server;
static request_handler _handler;
static temperature_reader _reader;

static char* get_route(char* request) {
    char* position = strstr(request, END_HEADER_REQUEST);
    int length = position - request;
    char* route = (char*) malloc(length + 1);
    memcpy(route, request, length);
    route[length] = '\0';
    return route;
}

static err_t on_receive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }
    char *route = get_route(p->payload);
    _handler(route);
    free(route);
    float temperature = _reader();
    char response[1024];
    snprintf(response, 1024, html, temperature);
    tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    pbuf_free(p);
    return ERR_OK;
}

static err_t on_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, on_receive);
    return ERR_OK;
}

int webserver_create(int port, request_handler handler, temperature_reader reader) {
    _server = tcp_new();
    if (!_server) return -1;
    if (tcp_bind(_server, IP_ADDR_ANY, port) != ERR_OK) return -1;
    _server = tcp_listen(_server);
    tcp_accept(_server, on_accept);
    _handler = handler;
    _reader = reader;
}