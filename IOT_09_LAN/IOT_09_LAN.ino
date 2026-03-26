//___________________
// ESP32 WEB SERVER - LED - RETURN DE DHT11
//___________________


//Bibliotecas
#include <WiFi.h> //ativação de módulo de wifi
#include <WebServer.h> //Ativação de servidor web

//Variáveis
const char* ssid = "nome da rede";
const char* password ="senha";


//login http
const char* http_user = "admin";
const char* http_password = "1234";

//Iniciando servidor
Webserver server(80);

//LED
const int LED_PIN=2; // Declarando GPIO 2
bool ledState = false; //Iniciando o led desligado

//Return DHT11
float temperatura =0;
float umidade = 0;

//conexão
bool wifiConnected = false;

//---------------------
// Autenticador
//---------------------
bool isAuthenticated(){
  if (!server.authenticate(http_user, http_password)){
    server.requestAuthenticator();
    return false;
  }
  return false;
}

//---------------------
// Front-end
//---------------------
String buildPage(){
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset = 'utf-8'>";
  html += "<meta name= 'viewport' content='width-device-width'>";
  html += "<title>ESP32</title>";
  html += "<style>";
  html += "body{font-family:sans-serif; text-align:center;padding:40px background:#f4f4f4}";
  html += ".on{background:#009d00; color:#f4f4f4}";
  html += ".off{background#43208;color:#f4f4f4}";
  html += "</head><body>";

  html += "<h1>Comunicação LAN das amizades</h1>";
  html += "<p>Status <strong>";
  html += (ledState) ? "LIGADO" : "DESLIGADO";
  html += </strong></p>;

  html += "<hr>";

  //botões
  html += "<a href = '/on'><button class='on'>LIGAR</button></a>"
  html += "<a href = '/off'><button class='off'>DESLIGAR</button></a>"

  //Retorno dos dados de TEMPERATURA
  html += "<h2>SENSOR DHT11</h2>";
  html += "<h2>Temperatura: ";
  html += String(temperatura);
  html += "%</p>"

  //retorno dos dados de UMIDADE
  html += "<h2> SENSOR DHT11</H2>";
  html += "<p>Umidade: ";
  html += String(umidade);
  html += "%</p>";

  html += "</body><html>";


  return html

}

//___________________
// WIFI
//___________________
bool connectWifi(){
  Serial.println("\nWifi connect... Resetando a interface");

  Wifi.disconnect(true);

  delay(1000);

  Wifi.mode(WIFI_STA);
  Wifi.begin(ssid, password);

  Serial.println("[Wifi] conectando...");
  Serial.print("[Wifi] SSID: ")
  Serial.println(ssid);

  int tentativas = 0; //contador de tentativas de conexão na rede

  //Tentativas de conexão
  while (WiFi.status() != WL_CONNECTED && tentativas < 20){
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED){ //WL_CONNECT?
    Serial.println("\n[WIFI] Conectado!!!");
    Serial.println("[WIFI] IP: ");
    Serial.println(WiFi.localIP()); // Retorna o IP, aqui acessaremos o servidor no navegador

    return true
  }else{
    Serial.println("\n[WIFI] Falha na conexão");
    return false;
  }
}

//___________________
// Rotas
//___________________
void setupRoutes(){
  //Se autenticado
  server.on("/", [](){
    if(!isAuthenticated()) return;
    server.send(200, "text/html", buildPage());
  });

  //LIGAR O LED
  server.on("/on", [](){
    if(!isAuthenticated()) return;
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED Ligado");
    server.sendHeader("Location", "/");
    server.send(303);
  })

  //DESLIGAR O LED
  server.on("/off", [](){
    if(!isAuthenticated()) return;
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED Desligado");
    server.sendHeader("Location", "/");
    server.send(303);
  });

  //RECEBER DADOS DO DHT11
  server.on("/update", [](){
    //SEM AUTH
    if(server.hasArg("temp")){
      temperatura = server.arg("temp").toFloat();
    }

    if(server.hasArg("hum")){
      umidade = server.arg("hum").toFloat();
    }

    Serial.println("[DHT] Dados recebidos");
    Serial.println("Temperatura: ");
    Serial.println(temperatura);
    Serial.println("Umidade: ");
    Serial.println(umidade);
    server.send(200,"text/plain", OK);
  });
}

//___________________
// Setup
//___________________
void setup() {
  Serial.begin(115200); //Importante checar se a sua placa não segue o 9600
  delay(1000);
  Serial.println("\n[BOOT] Inicializando o ESP32...")
  pintMode(LED_PINT,OUTPUT);
  digitalWrite(LED_PIN,LOW);

  wifiConnected = connectWifi();
  setupRoutes();

  if(wifiConnected){
    server.begin();
    Serial.println("[HTTP]Servidor Iniciado!");
  }else{
    Serial.println("[HTTP]Servidor NÃO iniciado (sem WIFI)!");
  }
}

//___________________
// Setup
//___________________
void loop() {
  if(WiFi.status()!= WL_CONNECTED){
    if(wifiConnected){
      Serial.println("[WIFI]Conexão Perdida");
      wifiConnected = false;
    }
    delay(2000);
    wifiConnected = connectWiFi();
    if (wifiConnected){
      server.begin();
      Serial.println("[HTTP]Servidor reiniciado!");
    }
    return;
  }
  server.handleClient();
}
