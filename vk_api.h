#include <cjson/cJSON.h>
#include <curl/curl.h>

#define VK_API_URL "https://api.vk.com/method/"

typedef struct
{
    char *access_token;
    char *api_version;
} VKAPI;

typedef struct
{
    char *server;
    char *key;
    char *ts;
} LongPollServer;

void vk_api_destroy(VKAPI *api);
VKAPI *vk_api_init(const char *access_token, const char *api_version);
cJSON *vk_api_call_method(VKAPI *api, const char *method, const char *params);
LongPollServer *vk_api_get_long_poll_server(VKAPI *api, int group_id);
cJSON *vk_api_long_poll_wait(LongPollServer *lp, int wait_time);
int vk_api_send_message(VKAPI *api, int peer_id, const char *message);