#include <MAX30105.h>
#include <Wire.h>
#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 16;      // Aumente para uma média maior
byte rates[RATE_SIZE];         // Armazena as últimas leituras
byte rateSpot = 0;
long lastBeat = 0;             // Momento do último batimento

float beatsPerMinute = 0;
int beatAvg = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("Initializing...");
  Wire.begin(21, 22); // SDA, SCL

    // Inicializa o sensor
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
    {
        Serial.println("MAX30105 was not found. Please check wiring/power.");
        while (1);
    }

    Serial.println("Place your index finger on the sensor with steady pressure.");

    // Configuração padrão do sensor
    particleSensor.setup();
    particleSensor.setPulseAmplitudeRed(0x0A);   // LED vermelho
    particleSensor.setPulseAmplitudeGreen(0);    // LED verde desligado
}

void loop()
{
    long irValue = particleSensor.getIR();   // Lê o valor do IR

    if (checkForBeat(irValue) == true)
    {
        // Calcula o tempo entre batimentos
        long delta = millis() - lastBeat;
        lastBeat = millis();

        // Calcula BPM
        beatsPerMinute = 60 / (delta / 1000.0);

        // Filtra leituras inválidas
        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
            rates[rateSpot++] = (byte)beatsPerMinute;
            rateSpot %= RATE_SIZE;

            // Calcula a média
            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
            {
                beatAvg += rates[x];
            }
            beatAvg /= RATE_SIZE;
        }
    }

    // Exibe os resultados
    Serial.print("IR=");
    Serial.print(irValue);

    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);

    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);

    if (irValue < 50000)
    {
        Serial.print(" No finger?");
    }

    Serial.println();
}