//___________________
// DHT11
//___________________


#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"


//DHT
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//Conexão de Wifi
const char* ssid = "nome da rede";
const char* password = "senha";

//ip SERVER
const char* serverIP = "192.168.1.0";

//controle de canal
unsigned long lastSend = 0;
const long interval = 5000;  //5 segundos

bool wifiConnected = false;

//___________________
// Wifi
//___________________
bool connectWifi() {
  Serial.println("\n[WIFI] Resetando a interface...");
  WiFi.disconnected(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\n[WIFI] conectando...");
  Serial.println("\n[WIFI]ssid: ");
  Serial.println(ssid);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.println(".");
    tentativas++
  }

  if (WiFi.status() == WL_CONNECTED) { 
    Serial.println("\n[WIFI] Conectado!!!");
    Serial.println("[WIFI] IP: ");
    Serial.println(WiFi.localIP());  // Retorna o IP, aqui acessaremos o servidor no navegador

    return true
  } else {
    Serial.println("\n[WIFI] Falha na conexão");
    return false;
  }
}
//___________________
// Envio HTTP
//___________________
void sendData(float temp, float hum){
  HTTPClient http;

  String url = ""
}

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}
