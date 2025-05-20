typedef void (*request_handler)(char* route);
typedef char* (*html_content)();

int webserver_create(int port, request_handler handler, html_content content);