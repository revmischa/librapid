#include <stdio.h>
#include <stdlib.h>
#include "api.h"

const char *server_address = "eastberkeley.int80";

int main() {
  rapid_api_ctx *ctx = rapid_api_alloc_context();
  rapid_api_set_server_address(ctx, server_address);

  if (rapid_api_connect(ctx)) {
    printf("Connected to %s\n", server_address);
  } else {
    exit(1);
  }

  rapid_api_disconnect(ctx);
}
