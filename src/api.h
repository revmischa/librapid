#ifndef __RAPID_API_H__
#define __RAPID_API_H__

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#include "message.h"
#include "jansson.h"

// default port to connect to a rapid server on
static const unsigned short RAPID_API_DEFAULT_PORT = 6000;

// how many bytes to try to read at a time
static const unsigned short RAPID_API_READ_BUF_SIZE = 1500;

typedef struct {
  char *server_address;
  unsigned short server_port;
  int socket;

  uint8_t *json_buf;
  uint32_t json_buf_len;
} rapid_api_ctx;

rapid_api_ctx* rapid_api_alloc_context();
void rapid_api_free_context(rapid_api_ctx* ctx);

void rapid_api_set_server_address(rapid_api_ctx* ctx, const char *addr);
void rapid_api_set_server_port(rapid_api_ctx* ctx, unsigned short port);

int rapid_api_connect(rapid_api_ctx* ctx);
void rapid_api_disconnect(rapid_api_ctx* ctx);

int rapid_api_read(rapid_api_ctx *ctx);
void rapid_api_read_handler(rapid_api_ctx *ctx, uint8_t *read_buf, ssize_t bytes_read);

#endif
