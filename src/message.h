#ifndef __RAPID_MESSAGE_H__
#define __RAPID_MESSAGE_H__

#include "jansson.h"
#include <string.h>	// memcpy
#include <stdlib.h>	// malloc

typedef struct {
  char *command;
  json_t *params;

  short params_copied;
  short command_copied;
  short params_ref;
} rapid_message;

rapid_message* rapid_alloc_message(void);
void rapid_free_message(rapid_message*);

short rapid_parse_message(json_t *json, rapid_message *msg);

void rapid_set_message_command(rapid_message *msg, char *command);
void rapid_set_message_command_copy(rapid_message *msg, const char *command);
void rapid_set_message_params(rapid_message *msg, json_t *);
void rapid_set_message_params_copy(rapid_message *msg, json_t *);
void rapid_set_message_params_ref(rapid_message *msg, json_t *);
void rapid_message_free_params(rapid_message *msg);

#endif
