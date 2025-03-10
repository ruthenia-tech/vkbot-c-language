#include "command_handler.h"
#include <stdlib.h>
#include <string.h>

CommandHandler *command_handler_init(void *context)
{
    CommandHandler *handler = (CommandHandler *)malloc(sizeof(CommandHandler));
    handler->commands = NULL;
    handler->commands_count = 0;
    handler->context = context;
    return handler;
}

void command_handler_add_command(CommandHandler *handler, const char *command, void (*handler_func)(void *, int, const char *))
{
    handler->commands = realloc(handler->commands, (handler->commands_count + 1) * sizeof(Command));
    handler->commands[handler->commands_count].command = command;
    handler->commands[handler->commands_count].handler = handler_func;
    handler->commands_count++;
}

void command_handler_process_message(CommandHandler *handler, int peer_id, const char *text)
{
    for (int i = 0; i < handler->commands_count; i++)
    {
        if (strncmp(text, handler->commands[i].command, strlen(handler->commands[i].command)) == 0)
        {
            handler->commands[i].handler(handler->context, peer_id, text);
            return;
        }
    }
    printf("Unknown command: %s\n", text);
}

void command_handler_destroy(CommandHandler *handler)
{
    free(handler->commands);
    free(handler);
}