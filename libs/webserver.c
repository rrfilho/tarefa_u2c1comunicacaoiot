#include "webserver.h"
#include <string.h>
#include <stdlib.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define END_HEADER_REQUEST "1.1"

static struct tcp_pcb *_server;
static request_handler _handler;
static html_content _content;

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
    char* response = _content();
    tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    pbuf_free(p);
    return ERR_OK;
}

static err_t on_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, on_receive);
    return ERR_OK;
}

int webserver_create(int port, request_handler handler, html_content content) {
    _server = tcp_new();
    if (!_server) return -1;
    if (tcp_bind(_server, IP_ADDR_ANY, port) != ERR_OK) return -1;
    _server = tcp_listen(_server);
    tcp_accept(_server, on_accept);
    _handler = handler;
    _content = content;
}