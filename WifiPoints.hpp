#ifndef GEOLOCATIONWIFIPOINTS_H_
#define GEOLOCATIONWIFIPOINTS_H_

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <wlanapi.h>
#include "cJSON.H"
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")


class WifiPoints
{
public:
    int getWifiNetworks();
    char* buildRequestJSON();
private:
    struct wifiAP {
        char macAddress[44];
        long signalStrength;
    };

    std::vector<wifiAP> wifiAPList;
    
    bool initializeWlanHandle(HANDLE& hClient, DWORD& dwCurVersion);
    bool processInterfaces(HANDLE hClient);
    bool processBssInfo(HANDLE hClient, PWLAN_INTERFACE_INFO pIfInfo);
    void processAccessPoint(PWLAN_BSS_ENTRY pBssEntry);
};

#endif
