#include "message.h"
#include "api.h"

rapid_api_ctx* rapid_api_alloc_context(void) {
  rapid_api_ctx *ctx = (rapid_api_ctx *)malloc(sizeof(rapid_api_ctx));
  if (! ctx) {
    printf("failed to alloc rapid api context!\n");
    return NULL;
  }
  //printf("alloc ctx=%lX, size=%u\n", ctx, sizeof(rapid_api_ctx));

  ctx->server_address = NULL;
  ctx->server_port = RAPID_API_DEFAULT_PORT;
  ctx->socket = 0;
  
  return ctx;
}

void rapid_api_free_context(rapid_api_ctx* ctx) {
  if (ctx->server_address) {
    free(ctx->server_address);
    ctx->server_address = NULL;
  }

  free(ctx);
  ctx = NULL;
}

void rapid_api_set_server_address(rapid_api_ctx* ctx, const char *addr) {
  if (ctx->server_address) {
    free(ctx->server_address);
    ctx->server_address = NULL;
  }

  ctx->server_address = malloc(strlen(addr) + 1);
  strcpy(ctx->server_address, addr);
}

void rapid_api_set_server_port(rapid_api_ctx* ctx, unsigned short port) {
  ctx->server_port = port;
}

// blocks, returns true if connected, false w/ errno if failure
int rapid_api_connect(rapid_api_ctx *ctx) {
  if (! ctx->server_address || ! ctx->server_port) {
    fprintf(stderr, "Warning: no server address or port set for rapid api context\n");
    return 0;
  }
  
  struct sockaddr_in sin;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  // resolve host name
  const char *hostname = ctx->server_address;
  struct hostent *h = gethostbyname(hostname);
  if (! h) {
    fprintf(stderr, "Failed to resolve address for host %s\n", hostname);
    return 0;
  }

  // got host info resolved
  memcpy(&sin.sin_addr.s_addr, h->h_addr, h->h_length);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(ctx->server_port);

  // try connecting
  if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    fprintf(stderr, "Failed to connect to %s on port %u: %s\n", hostname, ctx->server_port, strerror(errno));
    return 0;
  }

  ctx->socket = sock;

  return 1;
}

void rapid_api_disconnect(rapid_api_ctx *ctx) {
  if (! ctx->socket)
    return;

  shutdown(ctx->socket, SHUT_WR); // no more writes
}
