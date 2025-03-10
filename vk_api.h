#ifndef VK_API_H
#define VK_API_H

#include <cjson/cJSON.h>
#include <curl/curl.h>

#define VK_API_URL "https://api.vk.com/method/"

typedef struct
{
    char *key;
    char *value;
} VKAPIParam;

typedef struct
{
    VKAPIParam *params;
    size_t count;
} VKAPIParams;

typedef struct
{
    char *access_token;
    char *api_version;
    void *context;
} VKAPI;

typedef struct
{
    char *server;
    char *key;
    char *ts;
} LongPollServer;

void vk_api_destroy(VKAPI *api);
VKAPI *vk_api_init(const char *access_token, const char *api_version, void *context);
cJSON *vk_api_call_method(VKAPI *api, const char *method, VKAPIParams *params);
LongPollServer *vk_api_get_long_poll_server(VKAPI *api, int group_id);
cJSON *vk_api_long_poll_wait(LongPollServer *lp, int wait_time);
int vk_api_send_message(VKAPI *api, int peer_id, const char *message);

VKAPIParams *vk_api_params_create();
void vk_api_params_add(VKAPIParams *params, const char *key, const char *value);
void vk_api_params_free(VKAPIParams *params);
char *vk_api_params_to_string(VKAPIParams *params);

#endif