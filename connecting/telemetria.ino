// codigo base de conexão com redes 802.x de martinius96
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_wpa2.h"
#include <Wire.h>
#define EAP_IDENTITY "login" //login do Eduroam a ser definido
#define EAP_PASSWORD "password" //senha Eduroam a ser definida
const char* ssid = "eduroam"; // Eduroam SSID
WiFiClientSecure client;

void mkConnection(){
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
 esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY)); 
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.begin(ssid); //connect to wifi
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); //definir uma solução para quando não conseguir conectar
    }
  }
}
