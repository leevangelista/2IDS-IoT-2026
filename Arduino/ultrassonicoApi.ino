
// Bibliotecas
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <HCSR04.h>

// Wi-Fi
const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";

// URL da API
const char* serverName = "http://192.168.0.100/api/medidas";

// Definição dos pinos
const int pinoTrig = 12;
const int pinoEcho = 14;
const int pinoLed = 2;

// Sensor
UltraSonicDistanceSensor sensor(pinoTrig, pinoEcho);

float distancia;

// Controla se o POST já foi enviado
bool objetoDetectado = false;

// Envia os dados para a API
void enviarDados(String dado, String unidadeMedida, int sensorId) {

    // if (WiFi.status() != WL_CONNECTED) {
    //     Serial.println("WiFi desconectado.");
    //     return;
    // }

    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> doc;

    doc["dado"] = dado;
    doc["unidade_medida"] = unidadeMedida;
    doc["sensor_id"] = sensorId;

    String json;
    serializeJson(doc, json);

    Serial.println("Enviando:");
    Serial.println(json);

    // int httpCode = http.POST(json);

    Serial.print("Código HTTP: ");
    // Serial.println(httpCode);

    // if (httpCode > 0) {
    //     Serial.println(http.getString());
    // }

    http.end();
}

// Setup
void setup() {

    Serial.begin(115200);

    pinMode(pinoLed, OUTPUT);

    // Conecta ao Wi-Fi
    // WiFi.begin(ssid, password);

    Serial.print("Conectando ao WiFi");

    // while (WiFi.status() != WL_CONNECTED) {
    //     delay(500);
    //     Serial.print(".");
    // }

    Serial.println("\nWiFi conectado!");
}

// Loop
void loop() {

    // Lê a distância
    distancia = sensor.measureDistanceCm();

    Serial.print("Distância: ");
    Serial.print(distancia);
    Serial.println(" cm");

    // Objeto próximo
    if (distancia <= 20) {

        digitalWrite(pinoLed, HIGH);

        // Envia apenas uma vez enquanto o objeto estiver próximo
        if (!objetoDetectado) {

            objetoDetectado = true;

            enviarDados(
                String(distancia, 2),   // valor medido
                "cm",                   // unidade
                1                       // sensor_id
            );
        }

    } else {

        digitalWrite(pinoLed, LOW);

        // Permite novo envio quando o objeto sair da área
        objetoDetectado = false;
    }

    delay(100);
}