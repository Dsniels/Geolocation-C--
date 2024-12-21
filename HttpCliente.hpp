#ifndef HTTP_CLIENTE
#define HTTP_CLIENTE
#include "WifiPoints.hpp"
#include <vector>
#include <curl/curl.h>
#include "cJSON.h"

class HttpCliente
{
private:
    std::vector<wifiAP> accessPoints;
    struct Memory
    {
        char *Memory;
        size_t size;
    };

    static size_t allocateMemory(void *content, size_t size, size_t nmeb);

public:
    HttpCliente(std::vector<wifiAP> AccessPoints) : accessPoints(AccessPoints) {}
    ~HttpCliente();
    void getLocationResponse();
    char *buildRequest();
    void showLocation();
};


char *HttpCliente::buildRequest()
{
    cJSON *accessPointsObj = NULL;
    char *outputJson = NULL;
    cJSON *request = cJSON_CreateObject();



    if(cJSON_AddStringToObject(request,"considerIP","true") == NULL){
            goto end;
    }

    accessPointsObj = cJSON_AddArrayToObject(request, "wifiAccessPoints");

    for(auto & point: accessPoints){
        cJSON * AP = cJSON_CreateObject();
        if(cJSON_AddStringToObject(AP,"macAddress", point.macAddress) == NULL){
            goto end;
        }

        if(cJSON_AddNumberToObject(AP,"signalStregnth", point.signalStrength) == NULL){
            
            goto end;
        }

        cJSON_AddItemToArray(accessPointsObj,AP);

        outputJson = cJSON_Print(request);

        
    }


end:
    cJSON_Delete(request);

    return outputJson;
    
}

void HttpCliente::getLocationResponse(){
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
}


HttpCliente::~HttpCliente()
{
}

#endif