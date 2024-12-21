
#include "WifiPoints.hpp"
#include "HttpCliente.hpp"

using namespace std;

int main(){

    WifiPoints wifiAccess;
    auto wifiResult = wifiAccess.getWifiNetworks();

    if(!wifiResult.has_value()){
        cerr<<"Error: no se pudieron obtener redes wifi "<<endl;
        return 1;
    }

    auto &wifiAccessPoints = wifiResult.value();
    HttpCliente httpCliente(wifiAccessPoints);
    httpCliente.getLocationResponse();
}