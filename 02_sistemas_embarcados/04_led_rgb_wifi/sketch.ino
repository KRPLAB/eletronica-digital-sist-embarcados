#include <WiFi.h>
#include <WebServer.h>
#include <pgmspace.h>
#include <stdio.h>

// ==== 1. CONFIGURACOES WI-FI ====
const char *ssid = "roteadorKauan";
const char *password = "senha1234";

// ==== 2. DEFINICAO LEDS ====
const int red_led_pin = 2;
const int green_led_pin = 4;
const int blue_led_pin = 5;

// ==== 3. CRIACAO DO OBJETO DO SERVIDOR NA PORTA PADRAO 80 ====
WebServer server(80);

// ==== 4. PAGINA WEB (HTML) ====
const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 - Seletor de Cores</title>
    <style>
        :root {
            --background-color: #f0f2f5;
            --card-background: #ffffff;
            --text-color: #333333;
            --primary-color: #007bff;
            --primary-hover: #0056b3;
            --shadow-color: rgba(0, 0, 0, 0.1);
        }
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
            background-color: var(--background-color);
            color: var(--text-color);
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 1rem;
        }
        .color-card {
            background-color: var(--card-background);
            border-radius: 1rem;
            box-shadow: 0 4px 12px var(--shadow-color);
            padding: 2.5rem 2rem;
            width: 100%;
            max-width: 400px;
            text-align: center;
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }
        .color-card:hover {
            transform: translateY(-5px); 
            box-shadow: 0 8px 24px var(--shadow-color);
        }
        h1 {
            font-size: 1.75rem;
            margin-bottom: 1.5rem;
            color: var(--text-color);
        }
        label {
            font-size: 1.1rem;
            font-weight: 500;
            color: #555;
        }
        form {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 1.5rem;
        }
        input[type="color"] {
            appearance: none;
            -webkit-appearance: none;
            width: 80px;
            height: 80px;
            background-color: transparent;
            border: none;
            cursor: pointer;
        }
        input[type="color"]::-webkit-color-swatch {
            border-radius: 50%;
            border: 4px solid #eee;
            transition: border-color 0.2s ease;
        }
        input[type="color"]::-webkit-color-swatch:hover {
            border-color: #ddd;
        }
        .button {
            background-color: var(--primary-color);
            color: white;
            font-size: 1.1rem;
            font-weight: bold;
            padding: 0.9rem 2rem;
            border: none;
            border-radius: 0.5rem;
            cursor: pointer;
            text-transform: uppercase;
            letter-spacing: 1px;
            transition: background-color 0.3s ease, transform 0.2s ease;
        }
        .button:hover {
            background-color: var(--primary-hover);
            transform: scale(1.05);
        }
        .button:active {
            transform: scale(0.98);
        }
        @media (max-width: 480px) {
            .color-card {
                padding: 2rem 1.5rem;
            }
            h1 {
                font-size: 1.5rem;
            }
        }
    </style>
</head>
<body>
    <div class="color-card">
        <h1>Controle de LED 🎨</h1>
        <form action="/on" method="get">
            <label for="colorPicker">Toque no círculo para selecionar</label>
            <input type="color" id="colorPicker" name="cor" value="#ff0000">
            <button type="submit" class="button">Enviar Cor</button>
        </form>
    </div>
</body>
</html>
)rawliteral";

// ==== 5. FUNCOES DE CONTROLE ====

// Envia a pagina HTML para o navegador
void handleRoot()
{
    server.send(200, "text/html", MAIN_page);
}

// Controle de cor
void handleSetColor()
{
    String colorValue = "#ff0000";
    int r, g, b;

    if (server.hasArg("cor"))
    {
        colorValue = server.arg("cor");
    }

    Serial.print("Cor recebida: ");
    Serial.println(colorValue);

    // Converte a string de cor (#RRGGBB) para valores R, G, B
    // Usa c_str() para converter o objeto String para um const char*
    if (sscanf(colorValue.c_str() + 1, "%2x%2x%2x", &r, &g, &b) == 3)
    {
        analogWrite(red_led_pin, r);
        analogWrite(green_led_pin, g);
        analogWrite(blue_led_pin, b);
        Serial.printf("Valores RGB definidos: R=%d, G=%d, B=%d\n", r, g, b);
    }
    else
    {
        Serial.println("Formato de cor hexadecimal inválido.");
    }

    // Redireciona o navegador de volta para a pagina principal
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Cor atualizada!");
}

// ==== 6. CONFIGURACOES SETUP ====
void setup()
{
    Serial.begin(115200);

    // Configura os pinos dos LEDs como saída
    pinMode(red_led_pin, OUTPUT);
    pinMode(green_led_pin, OUTPUT);
    pinMode(blue_led_pin, OUTPUT);

    // Apaga os LEDs no início
    analogWrite(red_led_pin, 0);
    analogWrite(green_led_pin, 0);
    analogWrite(blue_led_pin, 0);

    // Conectando ao Wi-Fi
    Serial.print("Conectando a ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWi-Fi conectado!");
    Serial.print("Endereco de IP: ");
    Serial.println(WiFi.localIP());

    // Configurando as "rotas" do servidor
    server.on("/", HTTP_GET, handleRoot);
    server.on("/on", HTTP_GET, handleSetColor);
    server.begin();
    Serial.println("Servidor HTTP iniciado.");
}

// ==== 7. LOOP PRINCIPAL ====
void loop()
{
    server.handleClient();
}