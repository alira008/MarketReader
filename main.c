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

int main() {
  uv_loop_t loop;
  uv_loop_init(&loop);
  uv_tcp_t tcp_socket;
  uv_tcp_init(&loop, &tcp_socket);

  uv_connect_t request;

  struct sockaddr_in destination;
  uv_ip4_addr("127.0.0.1", 80, &destination);
  uv_tcp_connect(&request, &tcp_socket, (const struct sockaddr *)&destination,
                 on_tcp_connect);
  uv_run(&loop, UV_RUN_DEFAULT);
  printf("Finished work");
  return 0;
}
