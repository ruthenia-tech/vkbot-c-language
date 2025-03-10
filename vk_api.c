#include "vk_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VKAPI *vk_api_init(const char *access_token, const char *api_version, void *context)
{
    VKAPI *api = (VKAPI *)malloc(sizeof(VKAPI));
    api->access_token = strdup(access_token);
    api->api_version = strdup(api_version);
    api->context = context;
    return api;
}

void vk_api_destroy(VKAPI *api)
{
    free(api->access_token);
    free(api->api_version);
    free(api);
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t realsize = size * nmemb;
    char **response = (char **)userdata;
    *response = realloc(*response, realsize + 1);
    if (*response == NULL)
    {
        return 0;
    }
    memcpy(*response, ptr, realsize);
    (*response)[realsize] = 0;
    return realsize;
}

cJSON *vk_api_call_method(VKAPI *api, const char *method, VKAPIParams *params)
{
    CURL *curl;
    CURLcode res;
    char *response = NULL;
    char url[1024];

    char *params_str = vk_api_params_to_string(params);
    snprintf(url, sizeof(url), "%s%s?access_token=%s&v=%s&%s", VK_API_URL, method, api->access_token, api->api_version, params_str);
    free(params_str);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return NULL;
        }

        cJSON *json = cJSON_Parse(response);
        free(response);
        return json;
    }

    return NULL;
}

LongPollServer *vk_api_get_long_poll_server(VKAPI *api, int group_id)
{
    VKAPIParams *params = vk_api_params_create();
    vk_api_params_add(params, "group_id", "229693314");

    cJSON *response = vk_api_call_method(api, "groups.getLongPollServer", params);
    vk_api_params_free(params);

    if (!response)
    {
        return NULL;
    }

    cJSON *server_json = cJSON_GetObjectItemCaseSensitive(response, "response");
    if (!server_json)
    {
        cJSON_Delete(response);
        return NULL;
    }

    LongPollServer *lp = (LongPollServer *)malloc(sizeof(LongPollServer));
    lp->server = strdup(cJSON_GetObjectItemCaseSensitive(server_json, "server")->valuestring);
    lp->key = strdup(cJSON_GetObjectItemCaseSensitive(server_json, "key")->valuestring);
    lp->ts = strdup(cJSON_GetObjectItemCaseSensitive(server_json, "ts")->valuestring);

    cJSON_Delete(response);
    return lp;
}

cJSON *vk_api_long_poll_wait(LongPollServer *lp, int wait_time)
{
    CURL *curl;
    CURLcode res;
    char *response = NULL;
    char url[1024];

    snprintf(url, sizeof(url), "%s?act=a_check&key=%s&ts=%s&wait=%d", lp->server, lp->key, lp->ts, wait_time);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return NULL;
        }

        cJSON *json = cJSON_Parse(response);
        free(response);

        if (json)
        {
            cJSON *new_ts = cJSON_GetObjectItemCaseSensitive(json, "ts");
            if (new_ts)
            {
                free(lp->ts);
                lp->ts = strdup(new_ts->valuestring);
            }
        }

        return json;
    }

    return NULL;
}

int vk_api_send_message(VKAPI *api, int peer_id, const char *message)
{
    VKAPIParams *params = vk_api_params_create();
    char peer_id_str[16];
    snprintf(peer_id_str, sizeof(peer_id_str), "%d", peer_id);
    vk_api_params_add(params, "peer_id", peer_id_str);
    vk_api_params_add(params, "message", message);
    char random_id_str[16];
    snprintf(random_id_str, sizeof(random_id_str), "%d", rand());
    vk_api_params_add(params, "random_id", random_id_str);

    cJSON *response = vk_api_call_method(api, "messages.send", params);
    vk_api_params_free(params);

    if (!response)
    {
        return 0;
    }

    cJSON *error = cJSON_GetObjectItemCaseSensitive(response, "error");
    if (error)
    {
        fprintf(stderr, "Failed to send message: %s\n", cJSON_Print(error));
        cJSON_Delete(response);
        return 0;
    }

    cJSON_Delete(response);
    return 1;
}

VKAPIParams *vk_api_params_create()
{
    VKAPIParams *params = (VKAPIParams *)malloc(sizeof(VKAPIParams));
    params->params = NULL;
    params->count = 0;
    return params;
}

void vk_api_params_add(VKAPIParams *params, const char *key, const char *value)
{
    params->params = realloc(params->params, (params->count + 1) * sizeof(VKAPIParam));
    params->params[params->count].key = strdup(key);
    params->params[params->count].value = strdup(value);
    params->count++;
}

void vk_api_params_free(VKAPIParams *params)
{
    for (size_t i = 0; i < params->count; i++)
    {
        free(params->params[i].key);
        free(params->params[i].value);
    }
    free(params->params);
    free(params);
}

char *vk_api_params_to_string(VKAPIParams *params)
{
    char *result = strdup("");
    for (size_t i = 0; i < params->count; i++)
    {
        char *temp = result;
        asprintf(&result, "%s%s%s=%s", temp, i > 0 ? "&" : "", params->params[i].key, params->params[i].value);
        free(temp);
    }
    return result;
}