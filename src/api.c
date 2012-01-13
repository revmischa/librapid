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
  ctx->json_buf = NULL;
  
  return ctx;
}

void rapid_api_free_context(rapid_api_ctx* ctx) {
  if (ctx->server_address) {
    free(ctx->server_address);
    ctx->server_address = NULL;
  }

  if (ctx->json_buf) {
    free(ctx->json_buf);
    ctx->json_buf = NULL;
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
// nonblocking
// returns false on error
int rapid_api_read(rapid_api_ctx *ctx) {
  int sock = ctx->socket;
  if (! sock)
    return 0;

  uint32_t bufsize = RAPID_API_READ_BUF_SIZE;
  uint8_t *buf = malloc(bufsize);

  // read from socket
  ssize_t bytes_read;
  bytes_read = recv(sock, buf, bufsize, 0);
  if (bytes_read == -1) {
    if (errno == EAGAIN) {
      // no error, nothing to rad
      free(buf);
      return 1;
    } else {
      // read error
      perror("Error reading from socket");
      rapid_api_disconnect(ctx);
      free(buf);
      return 0;
    }
  }

  // i don't think this should ever happen
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

  // handle incremental parsing of JSON
  if (ctx->json_buf) {
    // append what we've read
    ctx->json_buf = realloc(ctx->json_buf, ctx->json_buf_len + bytes_read);
    memcpy((ctx->json_buf + ctx->json_buf_len), read_buf, bytes_read);
    ctx->json_buf_len += bytes_read;
  } else {
    ctx->json_buf = malloc(bytes_read);
    ctx->json_buf_len = bytes_read;
    memcpy(ctx->json_buf, read_buf, bytes_read);
  }
  
  json_t *root;
  json_error_t *json_err;
  // JSON_DISABLE_EOF_CHECK == allow extra data at the end of a valid JSON object
  root = json_loadb(ctx->json_buf, ctx->json_buf_len, JSON_DISABLE_EOF_CHECK, json_err);

  if (root == NULL) {
    // decoding failed
    // probably ok
    //printf("Failed to decode JSON\n");
    return;
  }

  // decoding success. remove the parsed json from the buffer
  // ????
  free(ctx->json_buf);
  ctx->json_buf = NULL;
  ctx->json_buf_len = 0;

  // we seem to have a valid JSON object now. let's turn it into a rapid message
  rapid_message *msg = rapid_alloc_message();
  if (rapid_parse_message(root, msg)) {
    printf("Got message with command: %s\n", msg->command);
  }

  rapid_free_message(msg);
}

    
    
