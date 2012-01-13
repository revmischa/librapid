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

  // set socket nonblocking
  int flags;
  if ((flags = fcntl(sock, F_GETFL, 0)) == -1)
    flags = 0;
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
  
  ctx->socket = sock;

  return 1;
}

void rapid_api_disconnect(rapid_api_ctx *ctx) {
  if (! ctx->socket)
    return;

  shutdown(ctx->socket, SHUT_WR); // no more writes
  ctx->socket = 0;
}

// poll socket, see if there is data to read
int rapid_api_read(rapid_api_ctx *ctx) {
  int sock = ctx->socket;
  if (! sock)
    return 0;

  uint32_t bufsize = 1500;
  uint8_t *buf = malloc(bufsize);

  ssize_t bytes_read;
  bytes_read = recv(sock, buf, bufsize, 0);
  if (bytes_read == -1) {
    // nothing to read?
    if (errno == EAGAIN) {
      free(buf);
      return 1;
    } else {
      perror("Error reading from socket");
      rapid_api_disconnect(ctx);
      free(buf);
      return 0;
    }
  }

  if (! bytes_read) {
    printf("Read 0 bytes!\n");
    free(buf);
    return 0;
  }

  // we've read some data, process it
  rapid_api_read_handler(ctx, buf, bytes_read);

  free(buf);

  return 1;
}

// called when we've read some data
// parse incoming data, call registered callbacks
void rapid_api_read_handler(rapid_api_ctx *ctx, uint8_t *read_buf, ssize_t bytes_read) {

  // buffer to hold json + NUL termination
  // FIXME: need to handle incremental parsing
  
  printf("Read data: %s\n", read_buf);

  json_t *root;
  json_error_t *json_err;
  // JSON_DISABLE_EOF_CHECK == allow extra data at the end of a valid JSON object
  root = json_loadb(read_buf, bytes_read, JSON_DISABLE_EOF_CHECK, json_err);

  if (root == NULL) {
    // decoding failed
    printf("Failed to decode JSON\n");
  }

  // we seem to have a valid JSON object now. let's turn it into a rapid message
  rapid_message *msg = rapid_alloc_message();
  if (rapid_parse_message(root, msg)) {
    printf("Got message with command: %s\n", msg->command);
  }

  rapid_free_message(msg);
}

    
    
