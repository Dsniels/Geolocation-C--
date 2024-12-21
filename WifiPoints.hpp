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

struct wifiAP
{
    char macAddress[44];
    long signalStrength;
};

class WifiPoints
{
public:
    int getWifiNetworks();

private:
    std::vector<wifiAP> wifiAPList;

    bool initializeWlanHandle(HANDLE &hClient, DWORD &dwCurVersion);
    bool processInterfaces(HANDLE hClient);
    bool processBssInfo(HANDLE hClient, PWLAN_INTERFACE_INFO pIfInfo);
    void processAccessPoint(PWLAN_BSS_ENTRY pBssEntry);
};



bool WifiPoints::initializeWlanHandle(HANDLE& hClient, DWORD& dwCurVersion) {
    DWORD dwResult = WlanOpenHandle(2, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "Error: WlanOpenHandle failed with error: " << dwResult << std::endl;
        return false;
    }
    return true;
}

void WifiPoints::processAccessPoint(PWLAN_BSS_ENTRY pBssEntry) {
    wifiAP accessPoint{"00:00:00:00:00:00", 0};
    sprintf(accessPoint.macAddress,
            "%02X:%02X:%02X:%02X:%02X:%02X",
            pBssEntry->dot11Bssid[0],
            pBssEntry->dot11Bssid[1],
            pBssEntry->dot11Bssid[2],
            pBssEntry->dot11Bssid[3],
            pBssEntry->dot11Bssid[4],
            pBssEntry->dot11Bssid[5]);

    accessPoint.signalStrength = pBssEntry->lRssi;
    std::cout << accessPoint.macAddress << std::endl;
    std::cout << accessPoint.signalStrength << std::endl;
    wifiAPList.push_back(accessPoint);
}


bool WifiPoints::processBssInfo(HANDLE hClient, PWLAN_INTERFACE_INFO pIfInfo) {
    PWLAN_BSS_LIST pBssList = NULL;
    DWORD dwResult = WlanGetNetworkBssList(hClient, &pIfInfo->InterfaceGuid, NULL, dot11_BSS_type_any, 0, 0, &pBssList);

    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "Error: WlanGetBssList failed with error: " << dwResult << std::endl;
        return false;
    }

    std::cout << std::endl << "Found " << pBssList->dwNumberOfItems << " access points" << std::endl;
    
    for (int k = 0; k < pBssList->dwNumberOfItems; k++) {
        processAccessPoint((PWLAN_BSS_ENTRY)&pBssList->wlanBssEntries[k]);
    }
    
    return true;
}

bool WifiPoints::processInterfaces(HANDLE hClient) {
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    DWORD dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);

    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "Error: WlanEnumInterface failed with error: " << dwResult << std::endl;
        return false;
    }

    for (int i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
        processBssInfo(hClient, (PWLAN_INTERFACE_INFO)&pIfList->InterfaceInfo[i]);
    }
    
    return true;
}

int WifiPoints::getWifiNetworks() {
    HANDLE hClient = NULL;
    DWORD dwCurVersion = 0;
    
    if (!initializeWlanHandle(hClient, dwCurVersion)) {
        return 1;
    }
    
    if (!processInterfaces(hClient)) {
        return 1;
    }
    
    return 0;
}



#endif
