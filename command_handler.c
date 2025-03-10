#include "command_handler.h"
#include <stdlib.h>
#include <string.h>

// Инициализация CommandHandler
CommandHandler *command_handler_init(void *context)
{
    CommandHandler *handler = (CommandHandler *)malloc(sizeof(CommandHandler));
    handler->commands = NULL;
    handler->commands_count = 0;
    handler->context = context;
    return handler;
}

// Добавление команды
void command_handler_add_command(CommandHandler *handler, const char *command, void (*handler_func)(void *, int, const char *))
{
    handler->commands = realloc(handler->commands, (handler->commands_count + 1) * sizeof(Command));
    handler->commands[handler->commands_count].command = command;
    handler->commands[handler->commands_count].handler = handler_func;
    handler->commands_count++;
}

// Обработка сообщения
void command_handler_process_message(CommandHandler *handler, int peer_id, const char *text)
{
    for (int i = 0; i < handler->commands_count; i++)
    {
        if (strncmp(text, handler->commands[i].command, strlen(handler->commands[i].command)) == 0)
        {
            // Вызов обработчика команды
            handler->commands[i].handler(handler->context, peer_id, text);
            return;
        }
    }
    // Если команда не найдена
    printf("Unknown command: %s\n", text);
}

// Освобождение ресурсов
void command_handler_destroy(CommandHandler *handler)
{
    free(handler->commands);
    free(handler);
}