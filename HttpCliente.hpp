#ifndef HTTP_CLIENTE
#define HTTP_CLIENTE
#include "WifiPoints.hpp"
#include <vector>
#include <curl/curl.h>
#include "cJSON.h"

class HttpCliente
{
public:
    HttpCliente(std::vector<wifiAP> AccessPoints) : accessPoints(AccessPoints) {}
    void getLocationResponse();
private:
    std::vector<wifiAP> accessPoints;
    struct Memory
    {
        char *Memory;
        size_t size;
    };
    void showLocation(char *response);
    static size_t allocateMemory(void *content, size_t size, size_t nmeb, void *userp);
    char *buildRequest();

};

void HttpCliente::showLocation(char *response)
{

    const cJSON *location = NULL;
    const cJSON *latitude = NULL;
    const cJSON *longitude = NULL;
    const cJSON *accuracy = NULL;

    cJSON *parseJSON = cJSON_Parse(response);

    if (!parseJSON)
    {
        const char *err = cJSON_GetErrorPtr();
        std::cerr << "Error: " << cJSON_GetErrorPtr() << std::endl;
        goto end;
    }

    location = cJSON_GetObjectItemCaseSensitive(parseJSON, "location");
    latitude = cJSON_GetObjectItemCaseSensitive(location, "lat");
    longitude = cJSON_GetObjectItemCaseSensitive(location, "lng");
    accuracy = cJSON_GetObjectItemCaseSensitive(parseJSON, "accuracy");
    if (location && longitude && latitude && accuracy)
    {
        std::cout << std::endl
                  << "You are in " << cJSON_Print(latitude) << "N " << cJSON_Print(longitude) << "E" << std::endl
                  << "within " << cJSON_Print(accuracy) << ' m' << std::endl;
    }

end:
    cJSON_Delete(parseJSON);
}

char *HttpCliente::buildRequest()
{
    cJSON *accessPointsObj = NULL;
    char *outputJson = NULL;
    cJSON *request = cJSON_CreateObject();

    if (cJSON_AddStringToObject(request, "considerIP", "true") == NULL)
    {
        goto end;
    }

    accessPointsObj = cJSON_AddArrayToObject(request, "wifiAccessPoints");

    for (auto &point : accessPoints)
    {
        cJSON *AP = cJSON_CreateObject();
        if (cJSON_AddStringToObject(AP, "macAddress", point.macAddress) == NULL)
        {
            goto end;
        }

        if (cJSON_AddNumberToObject(AP, "signalStregnth", point.signalStrength) == NULL)
        {

            goto end;
        }

        cJSON_AddItemToArray(accessPointsObj, AP);

        outputJson = cJSON_Print(request);
    }

end:
    cJSON_Delete(request);

    return outputJson;
}

size_t HttpCliente::allocateMemory(void *content, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = (char *)realloc(mem->Memory, mem->size + realsize + 1);

    if (!ptr)
    {
        std::cerr << "Error: allocateMemory failed" << std::endl;
        return 1;
    }

    mem->Memory = ptr;
    memcpy(&(mem->Memory[mem->size]), content, realsize);
    mem->size += realsize;
    mem->Memory[mem->size] = 0;

    return realsize;
}

void HttpCliente::getLocationResponse()
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    char *bodyPayload = buildRequest();

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    struct Memory chunk;
    chunk.Memory = NULL;
    chunk.size = 0;

    curl_easy_setopt(curl, CURLOPT_URL, "http://url");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyPayload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, allocateMemory);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    // perform the request and get a response code
    CURLcode response = curl_easy_perform(curl);

    if (response == CURLE_OK)
    {
        showLocation(chunk.Memory);
    }
    else
    {
        std::cerr << "Error: curl_perform failed " << std::endl;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    free(chunk.Memory);
}


#endif