#include <stdio.h>
#include <rapid_api.h>

int main() {
  printf("Hi!\n");

  rapid_message *rm = rapid_alloc_message();
  rapid_free_message(rm);
}
