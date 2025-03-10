#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "vk_api.h" // Подключаем заголовочный файл VKAPI

// Структура для хранения информации о команде
typedef struct
{
    const char *command;                                           // Название команды
    void (*handler)(void *context, int peer_id, const char *text); // Указатель на функцию-обработчик
} Command;

// Структура для обработчика команд
typedef struct
{
    Command *commands;  // Массив команд
    int commands_count; // Количество команд
    void *context;      // Контекст (например, указатель на VKAPI)
} CommandHandler;

// Инициализация CommandHandler
CommandHandler *command_handler_init(void *context);

// Добавление команды
void command_handler_add_command(CommandHandler *handler, const char *command, void (*handler_func)(void *, int, const char *));

// Обработка сообщения
void command_handler_process_message(CommandHandler *handler, int peer_id, const char *text);

// Освобождение ресурсов
void command_handler_destroy(CommandHandler *handler);

#endif // COMMAND_HANDLER_H