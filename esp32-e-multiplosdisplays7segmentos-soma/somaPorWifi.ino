/* Programa que faz a soma por interface web e wifi */

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "";
const char* password = "";

WebServer server(80);
int resultado = 0;

void handleRoot() {
  String html = R"rawliteral(
    <html lang="pt-BR">
    <head>
        <meta charset="UTF-8">
    </head>
    <body>
        <h2>Somador de 2 números</h2>
        <form action="/soma">
            Número 1: <input type="number" name="a"><br>
            Número 2: <input type="number" name="b"><br>
            <input type="submit" value="Somar">
        </form>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleSoma() {
  if (server.hasArg("a") && server.hasArg("b")) {
    int a = server.arg("a").toInt();
    int b = server.arg("b").toInt();
    resultado = a + b;

    Serial.printf("Recebido: %d + %d = %d\n", a, b, resultado);
  }

  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConectado! IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/soma", handleSoma);
  server.begin();
  Serial.println("Servidor iniciado.");
}

void loop() {
  server.handleClient();
}