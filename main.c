#include "command_handler.h"
#include "vk_api.h"
#include "commands.h"

int main()
{
    // Инициализация VKAPI
    VKAPI *api = vk_api_init("faker", "5.199");

    // Инициализация CommandHandler
    CommandHandler *handler = command_handler_init(api);

    // Регистрация команд
    command_handler_add_command(handler, "/start", handle_start);
    command_handler_add_command(handler, "/help", handle_help);
    command_handler_add_command(handler, "/time", handle_time);

    // Пример обработки сообщений
    command_handler_process_message(handler, 12345, "/start");
    command_handler_process_message(handler, 12345, "/time");
    command_handler_process_message(handler, 12345, "/unknown"); // Неизвестная команда

    // Освобождение ресурсов
    command_handler_destroy(handler);
    vk_api_destroy(api);

    return 0;
}