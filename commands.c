#include "command_handler.h"
#include "vk_api.h"
#include <stdio.h>
#include <time.h>

// Пример обработчика команды /start
void handle_start(void *context, int peer_id, const char *text) {
    VKAPI *api = (VKAPI *)context;
    const char *reply_message = "Привет! Я бот. Вот список доступных команд:\n/help - Показать список команд\n/time - Показать текущее время";
    vk_api_send_message(api, peer_id, reply_message);
}

// Пример обработчика команды /help
void handle_help(void *context, int peer_id, const char *text) {
    VKAPI *api = (VKAPI *)context;
    const char *reply_message = "Доступные команды:\n/help - Показать список команд\n/time - Показать текущее время";
    vk_api_send_message(api, peer_id, reply_message);
}

// Пример обработчика команды /time
void handle_time(void *context, int peer_id, const char *text) {
    VKAPI *api = (VKAPI *)context;
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    char reply_message[128];
    snprintf(reply_message, sizeof(reply_message), "Текущее время: %s", time_str);
    vk_api_send_message(api, peer_id, reply_message);
}