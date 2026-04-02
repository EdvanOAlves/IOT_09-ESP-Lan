//___________________
// DHT11
//___________________


#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>


//DHT
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//Conexão de Wifi
const char* ssid = "IoT-B08";
const char* password = "12345678";

//ip SERVER
const char* serverIP = "192.168.0.112";

//controle de canal
unsigned long lastSend = 0;
const long interval = 5000;  //5 segundos

bool wifiConnected = false;

//___________________
// Wifi
//___________________
bool connectWiFi() {
  Serial.println("\n[WIFI] Resetando a interface...");
  WiFi.disconnect(true);
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
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) { 
    Serial.println("\n[WIFI] Conectado!!!");
    Serial.println("[WIFI] IP: ");
    Serial.println(WiFi.localIP());  // Retorna o IP, aqui acessaremos o servidor no navegador

    return true;
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
  String url = "http://"+String(serverIP)+
    "update?temp=" + String(temp,1) +
    "&hum" + String(hum,1);
  
  Serial.println("[HTTP]Enviando: ");
  Serial.println(url);

  http.begin(url);
  http.setTimeout(3000); //3 segs

  int httpCode = http.GET();

  if (httpCode > 0){

    Serial.println("[HTTP] Código: ");
    Serial.println(httpCode);

    String payload = http.getString();
    Serial.print("[HTTP] Resposta: ");
    Serial.println(payload);

  }else{
    Serial.println("[HTTP]ERROR: ");
    Serial.println(httpCode);
  }
  http.end();
}

//___________________
// Setup
//___________________


void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n[BOOT] Inicializando DHT Client...");

  dht.begin();

  wifiConnected = connectWiFi();
}

//___________________
// Loop
//___________________

void loop() {
  // Reconexão WiFi
  if (WiFi.status() != WL_CONNECTED){
    
    if (wifiConnected){
      Serial.println("[WiFi] Conexão perdida!");
      wifiConnected = false;
    }

    delay(2000);
    wifiConnected = connectWiFi();
    return;
  }

  // Envio periódico (sem delay travando)

  unsigned long currentMillis = millis();
  if (currentMillis - lastSend >= interval){
    
    lastSend = currentMillis;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)){
      Serial.println("[DHT] Falha na leitura");
      return;
    }

    Serial.print("DHT] Temp: ");
    Serial.print(temp);
    Serial.print(" °C | Umidade: ");
    Serial.println(hum);

    sendData(temp, hum);
  }
}
