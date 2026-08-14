
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

// Controla se o POST já foi enviado
bool objetoDetectado = false;

// FAZER AQUI A DECLARAÇÃO DAS VARIÁVEIS NECESSÁRIAS PARA O SENSOR


// Envia os dados para a API
void enviarDados(String dado, String unidadeMedida, int sensorId) {

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi desconectado.");
        return;
    }

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

    if (httpCode > 0) {
        Serial.println(http.getString());
    }

    http.end();
}

// Setup
void setup() {

    Serial.begin(115200);

    // iniciar aqui variaveis do sensor caso necessário

    // Conecta ao Wi-Fi
    WiFi.begin(ssid, password);

    Serial.print("Conectando ao WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado!");
}

// Loop
void loop() {

    // INSERIR O CODIGO REFERENTE A REGRA DO SENSOR
    // ENVIAR OS DADOS PARA FUNCAO enviarDados(dado, unidadeMedida, sensorId)
    // OBS pegar o sensorId fixo igual o número que estiver no banco de dados
}