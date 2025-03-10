#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "vk_api.h"

typedef struct
{
    const char *command;
    void (*handler)(void *context, int peer_id, const char *text);
} Command;

typedef struct
{
    Command *commands;
    int commands_count;
    void *context;
} CommandHandler;

CommandHandler *command_handler_init(void *context);
void command_handler_add_command(CommandHandler *handler, const char *command, void (*handler_func)(void *, int, const char *));
void command_handler_process_message(CommandHandler *handler, int peer_id, const char *text);
void command_handler_destroy(CommandHandler *handler);

#endif