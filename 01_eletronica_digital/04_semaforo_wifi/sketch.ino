#include <WiFi.h>
#include <WebServer.h>

#define AMARELO 16
#define VERMELHO 18
#define VERDE 19

// --- credenciais do Wi-Fi ---
const char* ssid = "";
const char* password = "";
// ------------------------------------

// criar uma instancia do servidor na porta 80 (padrao para HTTP)
WebServer server(80);

void liga_vermelho() {
  digitalWrite(AMARELO, LOW);
  digitalWrite(VERDE, LOW);
  digitalWrite(VERMELHO, HIGH);
  server.send(200, "text/plain", "LED aceso!"); 
}

void liga_amarelo() {
  digitalWrite(VERMELHO, LOW);
  digitalWrite(VERDE, LOW);
  digitalWrite(AMARELO, HIGH);
  server.send(200, "text/plain", "LED aceso!"); 
}

void liga_verde() {
  digitalWrite(AMARELO, LOW);
  digitalWrite(VERMELHO, LOW);
  digitalWrite(VERDE, HIGH);
  server.send(200, "text/plain", "LED aceso!"); 
}

// Funcao que cria e envia a pagina HTML para o navegador
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>Controle do ESP32</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "html { font-family: sans-serif; text-align: center; }";
  html += "body { background-color: #282c34; color: white; }";
  html += "h1 { color: #61dafb; }";
  html += ".button { background-color: #EE1310; border: none; color: #282c34; padding: 16px 40px;";
  html += "text-decoration: none; font-size: 30px; margin: 10px; cursor: pointer; }";
  html += ".button:active { background-color: #10EAEE; }";
  html += "</style></head>";
  html += "<body><h1>Controle do LED</h1>";
  html += "<p><a href='/vermelho'><button class='button'>VERMELHO</button></a></p>";
  html += "<p><a href='/verde'><button class='button'>VERDE</button></a></p>";
  html += "<p><a href='/amarelo'><button class='button'>AMARELO</button></a></p>";
  html += "</body></html>";

  // Envia a pagina para o navegador
  server.send(200, "text/html", html);
}


void setup() {
  Serial.begin(115200);
  pinMode(VERMELHO, OUTPUT);
  pinMode(AMARELO, OUTPUT);
  pinMode(VERDE, OUTPUT);
  
  // Garante que o LED comece desligado
  digitalWrite(VERMELHO, LOW); 
  digitalWrite(AMARELO, LOW);
  digitalWrite(VERDE, LOW);

  // --- Conexao Wi-Fi ---
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.print("Endereco de IP: ");
  
  // Imprime o IP do ESP32
  Serial.println(WiFi.localIP());
  // -----------------------

  // --- Configuraao do Servidor Web ---
  server.on("/", handleRoot);   
  server.on("/vermelho", liga_vermelho);     
  server.on("/verde", liga_verde);
  server.on("/amarelo", liga_amarelo);

  // Inicia o servidor
  server.begin(); 
  Serial.println("Servidor web iniciado!");
  // ------------------------------------
}

void loop() {
  server.handleClient();
}