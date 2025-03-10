#ifndef COMMANDS_H
#define COMMANDS_H

#include "command_handler.h"

// Объявления функций для обработки команд
void handle_start(void *context, int peer_id, const char *text);
void handle_help(void *context, int peer_id, const char *text);
void handle_time(void *context, int peer_id, const char *text);

#endif // COMMANDS_H