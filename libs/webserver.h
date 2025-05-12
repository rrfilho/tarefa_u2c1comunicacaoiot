typedef void (*request_handler)(char* route);
typedef float (*temperature_reader)();

int webserver_create(int port, request_handler handler, temperature_reader reader);