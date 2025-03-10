#include "command_handler.h"
#include "vk_api.h"
#include "commands.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void handle_new_message(VKAPI *api, cJSON *object) {
    if (!object) {
        fprintf(stderr, "Object is NULL\n");
        return;
    }

    cJSON *message = cJSON_GetObjectItemCaseSensitive(object, "message");
    if (!message) {
        fprintf(stderr, "Message field is missing\n");
        return;
    }

    cJSON *text = cJSON_GetObjectItemCaseSensitive(message, "text");
    cJSON *from_id = cJSON_GetObjectItemCaseSensitive(message, "from_id");
    cJSON *peer_id = cJSON_GetObjectItemCaseSensitive(message, "peer_id");

    if (text && from_id && peer_id) {
        printf("New message from %d in chat %d: %s\n", from_id->valueint, peer_id->valueint, text->valuestring);

        CommandHandler *handler = (CommandHandler *)api->context;
        command_handler_process_message(handler, peer_id->valueint, text->valuestring);
    } else {
        fprintf(stderr, "Missing fields in message\n");
    }
}

int main() {
    VKAPI *api = vk_api_init("faker", "5.199", NULL);
    if (!api) {
        fprintf(stderr, "Failed to initialize VKAPI\n");
        return 1;
    }

    CommandHandler *handler = command_handler_init(api);
    if (!handler) {
        fprintf(stderr, "Failed to initialize CommandHandler\n");
        vk_api_destroy(api);
        return 1;
    }

    api->context = handler;

    command_handler_add_command(handler, "/start", handle_start);
    command_handler_add_command(handler, "/help", handle_help);
    command_handler_add_command(handler, "/time", handle_time);

    LongPollServer *lp = vk_api_get_long_poll_server(api, 229693314);
    if (!lp) {
        fprintf(stderr, "Failed to get Long Poll server\n");
        command_handler_destroy(handler);
        vk_api_destroy(api);
        return 1;
    }

    printf("Long Poll Server: %s\n", lp->server);
    printf("Long Poll Key: %s\n", lp->key);
    printf("Long Poll TS: %s\n", lp->ts);

    while (1) {
        cJSON *updates = vk_api_long_poll_wait(lp, 25);
        if (!updates) {
            fprintf(stderr, "Failed to get updates\n");
            continue;
        }

        cJSON *updates_array = cJSON_GetObjectItemCaseSensitive(updates, "updates");
        if (!updates_array) {
            fprintf(stderr, "No updates found in response\n");
            cJSON_Delete(updates);
            continue;
        }

        for (int i = 0; i < cJSON_GetArraySize(updates_array); i++) {
            cJSON *update = cJSON_GetArrayItem(updates_array, i);
            cJSON *type = cJSON_GetObjectItemCaseSensitive(update, "type");

            if (type && strcmp(type->valuestring, "message_new") == 0) {
                cJSON *object = cJSON_GetObjectItemCaseSensitive(update, "object");
                if (object) {
                    handle_new_message(api, object);
                }
            }
        }

        cJSON_Delete(updates);
    }

    free(lp->server);
    free(lp->key);
    free(lp->ts);
    free(lp);
    command_handler_destroy(handler);
    vk_api_destroy(api);

    return 0;
}