#include "message.h"

short rapid_parse_message(json_t *json, rapid_message *msg) {
  json_t *command, *params;

  if (! msg) return 0;

  if (! json_is_object(json)) return 0;

  command = json_object_get(json, "command");
  if (command == NULL || ! json_is_string(command))
    return 0;

  params = json_object_get(json, "params");
	
  // cool

  rapid_set_message_command_copy(msg, json_string_value(command));
                          
  if (params) {
    //rapid_set_message_params_ref(msg, params);
  }

  return 1;	
}						

rapid_message* rapid_alloc_message(void) {
  rapid_message *msg = (rapid_message *)malloc(sizeof(rapid_message));
  if (! msg) {
    printf("failed to alloc msg!\n");
    return NULL;
  }
													 
  msg->command = NULL;
  msg->params  = NULL;
  msg->params_copied  = 0;
  msg->command_copied = 0;
  msg->params_ref     = 0;

  return msg;	
}

void rapid_free_message(rapid_message *msg) {
  if (msg->command_copied && msg->command)
    free(msg->command);

  rapid_message_free_params(msg);

  free(msg);
}
		
void rapid_set_message_command(rapid_message *msg, char *command) {
  if (msg->command_copied && msg->command)
    free(msg->command);

  msg->command = command;
  msg->command_copied = 0;
} 
		
void rapid_set_message_command_copy(rapid_message *msg, const char *command) {
  //if (msg->command == command) return;
  char *copy;

  if (msg->command_copied && msg->command)
    free(msg->command);
	
  copy = strdup(command);
  msg->command = copy;

  msg->command_copied = 1;
}

void rapid_set_message_params(rapid_message *msg, json_t *params) {
  rapid_message_free_params(msg);

  msg->params = params;
  msg->params_copied = 0;
} 

void rapid_set_message_params_copy(rapid_message *msg, json_t *params) {
  rapid_message_free_params(msg);

  msg->params = json_deep_copy(params);
  msg->params_copied = 1;
}

void rapid_set_message_params_ref(rapid_message *msg, json_t *params) {
  rapid_message_free_params(msg);

  json_incref(params);
  msg->params_ref = 1;

  msg->params = params;	
}

void rapid_message_free_params(rapid_message *msg) {
  if (msg->params_copied && msg->params)
    free(msg->params);
  else if (msg->params_ref && msg->params)
    json_decref(msg->params);

  msg->params_copied = 0;
  msg->params_ref    = 0;
  msg->params        = NULL;
}
