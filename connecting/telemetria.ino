// codigo base de conexão com redes 802.x de martinius96
// ======== BIBLIOTECAS ========
#include "ThingSpeak.h"
#include "DHT.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_wpa2.h"
#include <Wire.h>

// ======== DEFINIÇÕES ======== 
#define pino_iPV 36 /*isso é um valor qualquer. adicionar valor definitivo*/
#define pino_vPV 39 /*isso é um valor qualquer. adicionar valor definitivo*/
#define pino_iBat 34 /*isso é um valor qualquer. adicionar valor definitivo*/
#define pino_vBat 35 /*isso é um valor qualquer. adicionar valor definitivo*/
#define pino_iAC 32 /*isso é um valor qualquer. adicionar valor definitivo*/
#define pino_vAC 33 /*isso é um valor qualquer. adicionar valor definitivo*/
#define pino_temp 25 /*isso é um valor qualquer. adicionar valor definitivo*/
#define EAP_ANONYMOUS_IDENTITY ""
#define EAP_IDENTITY "login" //login do Eduroam a ser definido
#define EAP_PASSWORD "password" //senha Eduroam a ser definida
const char* ssid = "eduroam"; // Eduroam SSID
WiFiClientSecure client;
unsigned long valorAtualChannel = 000000; // ID de canal valor atual
unsigned long valorMediaChannel = 000000; // ID de canal valor média
const char * myWriteAPIKey = "API KEY";

// configuracao DHT
#define tipo_DHT DHT22
DHT dht(pino_temp,tipo_DHT);

// ======== VARIAVEIS ========
// medidas instantaneas
float iPV; //corrente do modulo fotovoltaico
float vPV; //tensao do modulo fotovoltaico
float iBat; //corrente da bateria
float vBat; //tensão da bateria
float iAC; //corrente de saída do inversor (circuito AC)
float vAC; //tensão de saída do inversor (circuito AC)
float temp; //temperatura ambiente da caixa
// medias
float media_iPV = 0; //media da corrente do modulo fotovoltaico em T min
float media_vPV = 0; //media da tensao do modulo fotovoltaico em T min
float media_iBat = 0; //media da corrente da bateria em T min
float media_vBat = 0; //media da tensão da bateria em T min
float media_iAC = 0; //media da corrente de saída do inversor (circuito AC) em T min
float media_vAC = 0; //media da tensão de saída do inversor (circuito AC) em T min
float media_temp = 0; //media da temperatura ambiente da caixa em T min
// razoes de conversao 
const float razao_iPV = 1; /*isso é um valor qualquer. adicionar valor definitivo que dependera do AMPLIFICADOR DE TENSAO, caso haja*/
const float razao_vPV = 1; /*isso é um valor qualquer. adicionar valor definitivo que dependera do DIVISOR DE TENSAO, caso haja*/
const float razao_iBat = 1; /*isso é um valor qualquer. adicionar valor definitivo que dependera do AMPLIFICADOR DE TENSAO, caso haja*/
const float razao_vBat = 1; /*isso é um valor qualquer. adicionar valor definitivo que dependera do DIVISOR DE TENSAO, caso haja*/
const float razao_iAC = 1; /*isso é um valor qualquer. adicionar valor definitivo que dependera do AMPLIFICADOR DE TENSAO, caso haja*/
const float razao_vAC = 1; /*isso é um valor qualquer. adicionar valor definitivo que dependera do DIVISOR DE TENSAO, caso haja*/
// tempos e contadores
unsigned long T = 600000; //periodo de calculo da media: 600000 ms = 600 s = 10 min
unsigned long tempo = 0;
int i = 0; //conta quantas leituras houve dentro do periodo de calculo da media

// ======== FUNCOES ========
float ler_v(int pino, float razao){
  float v = (analogRead(pino)*3.3/1023)*razao;
  return v;
}

float ler_i(int pino, float razao){ 
  //DATASHEET NAO DEIXA CLARO QUAL O MINIMO E MAXIMO DA TENSÃO DE SAIDA DO SENSOR, JA QUE E UM SENSOR BIDIRECIONAL. APENAS 20mV = 1A.
  //SE -100A = 0V & 100V = Vcc, O CODIGO ESTA CERTO. SE -100A = -Vcc & 100V = Vcc, O CODIGO DEVE SER ALTERADO E UM CIRCUITO AUXILIAR DEVE SER ADICIONADO
  //CONSIDERANDO 0A = Vcc/2 = 3.3/2 = 1.65V
  float i = ((analogRead(pino)*3.3/1023)-1.65)/(0.02*razao);
  return i;
}

void calcular_media(){
  media_iPV = media_iPV/i;
  media_vPV = media_vPV/i;
  media_iBat = media_iBat/i;
  media_vBat = media_vBat/i;
  media_iAC = media_iAC/i; 
  media_vAC = media_vAC/i;
  media_temp = media_temp/i;
}

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


void startThingspeak(){

}

void leituraSensores(){
  iPV = ler_i(pino_iPV,razao_iPV);
  media_iPV = media_iPV + iPV;
  
  vPV = ler_v(pino_vPV,razao_vPV);
  media_vPV = media_vPV + vPV;
  
  iBat = ler_i(pino_iBat,razao_iBat);
  media_iBat = media_iBat + iBat;
  
  vBat = ler_v(pino_vBat,razao_vBat);
  media_vBat = media_vBat + vBat;
  
  iAC = ler_i(pino_iAC,razao_iAC);
  media_iAC = media_iAC + iAC;
  
  vAC = ler_v(pino_vAC,razao_vAC);
  media_vAC = media_vAC + vAC;
  
  temp = dht.readTemperature();
  media_temp = media_temp + temp;
}

void thingspeakWrite(){
  //atual
  ThingSpeak.writeField(valorAtualChannel, 1, iPV, myWriteAPIKey);
  ThingSpeak.writeField(valorAtualChannel, 2, vPV, myWriteAPIKey);
  ThingSpeak.writeField(valorAtualChannel, 3, iBat, myWriteAPIKey);
  ThingSpeak.writeField(valorAtualChannel, 4, vBat, myWriteAPIKey);
  ThingSpeak.writeField(valorAtualChannel, 5, iAC, myWriteAPIKey);
  ThingSpeak.writeField(valorAtualChannel, 6, vAC, myWriteAPIKey);
  ThingSpeak.writeField(valorAtualChannel, 7, temp, myWriteAPIKey);
  //média
  ThingSpeak.writeField(valorMediaChannel, 1, media_iPV, myWriteAPIKey);
  ThingSpeak.writeField(valorMediaChannel, 2, media_vPV, myWriteAPIKey);
  ThingSpeak.writeField(valorMediaChannel, 3, media_iBat, myWriteAPIKey);
  ThingSpeak.writeField(valorMediaChannel, 4, media_vBat, myWriteAPIKey);
  ThingSpeak.writeField(valorMediaChannel, 5, media_iAC, myWriteAPIKey);
  ThingSpeak.writeField(valorMediaChannel, 6, media_vAC, myWriteAPIKey);
  ThingSpeak.writeField(valorMediaChannel, 7, media_temp, myWriteAPIKey);
}

void setup() 
{
  dht.begin();
  mkConnection();
  ThingSpeak.begin(client);
}

void loop(){

/*A cada 15 s, realiza as leituras e salva nas respecitvas variaveis*/  
  leituraSensores();
  i++; //conta quantas leituras foram feitas
  delay(15000); //para o codigo por 15 s

/*checa se a funcao millis() reiniciou (o que ocorrera a cada 30 dias) e zera o contador de tempo*/
  if (tempo > millis()){
    tempo=0;
  }

/*caso tenha passado 10 min do ultimo envio de dados, as medias sao calculadas e os valores enviados para o servidor*/
  if (millis() - tempo >= T){
    calcular_media();
    thingspeakWrite();
    tempo = millis(); //registra o tempo em que ocorreu o envio de dados 
    i = 0; //reinicia o contador
  }
}
