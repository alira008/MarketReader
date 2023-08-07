#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <uv.h>

void on_tcp_write(uv_write_t *req, int status) {
  if (status < 0) {
    printf("Could not write to tcp socket\n");
  }
  printf("Sent message\n");
}

void on_tcp_connect(uv_connect_t *req, int status) {
  if (status < 0) {
    printf("Could not connect to server\n");
    return;
  }

  uv_write_t write_req;
  const uv_buf_t buf[] = {{.base = "hello\0", .len = 6}};
  uv_write(&write_req, req->handle, buf, 1, on_tcp_write);

  printf("Connected\n");
}

void on_address_parse(const struct sockaddr *destination) {
  printf("Connecting to server.\n");

  // initialize tcp handle
  uv_loop_t *loop = uv_default_loop();
  uv_tcp_t tcp_handle;
  uv_connect_t request;
  uv_tcp_init(loop, &tcp_handle);

  uv_tcp_connect(&request, &tcp_handle, destination, on_tcp_connect);
}

void on_address_resolve(uv_getaddrinfo_t *req, int status,
                        struct addrinfo *res) {
  if (status < 0) {
    printf("Could not resolve address\n");
    free(req->data);
    free(req);
    uv_freeaddrinfo(res);
    return;
  }

  struct addrinfo *addr;
  struct sockaddr *destination = NULL;

  // Loop through addresses
  for (addr = res; addr != NULL; addr = addr->ai_next) {
    if (addr->ai_family == AF_INET) {
      destination = addr->ai_addr;
    }
  }

  if (destination != NULL) {
    on_address_parse(destination);
  }

  // Free ptrs
  uv_freeaddrinfo(res);
  free(req);
}

void on_server_connected(const char *ip, int port) {
  printf("Address is: %s, Port: %d\n", ip, port);
}

void connect_to_server(const char *host) {
  // Resolve the host address
  uv_loop_t *loop = uv_default_loop();
  uv_getaddrinfo_t *resolver =
      (uv_getaddrinfo_t *)malloc(sizeof(uv_getaddrinfo_t));
  uv_getaddrinfo(loop, resolver, on_address_resolve, host, "https", NULL);
}

int main() {
  uv_loop_t *loop = uv_default_loop();
  connect_to_server("paper-api.alpaca.markets");

  uv_run(loop, UV_RUN_DEFAULT);
  printf("Finished work");
  return 0;
}
