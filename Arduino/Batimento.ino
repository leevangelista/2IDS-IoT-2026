#include <MAX30105.h>
#include "heartRate.h"
#include "spo2_algorithm.h"

// =====================================================
// ESP32 + MAX30102
// Biblioteca: SparkFun MAX3010x Sensor Library
// Necessário:
//   - MAX30105.h
//   - heartRate.h
//   - spo2_algorithm.h / spo2_algorithm.cpp
// =====================================================

MAX30105 sensor;

const byte SDA_PIN = 21;
const byte SCL_PIN = 22;

const byte BUFFER_SIZE = 100;

uint32_t irBuffer[BUFFER_SIZE];
uint32_t redBuffer[BUFFER_SIZE];

int32_t spo2;
int8_t validSPO2;

int32_t heartRateCalc;
int8_t validHeartRate;

// ---- Média do BPM ----
const byte RATE_SIZE = 8;
byte rates[RATE_SIZE];
byte rateSpot = 0;

long lastBeat = 0;
float bpm = 0;
int bpmMedia = 0;

void preencherBuffer()
{
  for (byte i = 0; i < BUFFER_SIZE; i++)
  {
    while (!sensor.available())
      sensor.check();

    redBuffer[i] = sensor.getRed();
    irBuffer[i] = sensor.getIR();

    sensor.nextSample();
  }
}

void calcularSpo2()
{
  maxim_heart_rate_and_oxygen_saturation(
      irBuffer,
      BUFFER_SIZE,
      redBuffer,
      &spo2,
      &validSPO2,
      &heartRateCalc,
      &validHeartRate);
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!sensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX30102 nao encontrado.");
    while (1);
  }

  Serial.println("MAX30102 iniciado.");

  sensor.setup();
  sensor.setPulseAmplitudeRed(0x1F);
  sensor.setPulseAmplitudeGreen(0);

  preencherBuffer();
  calcularSpo2();
}

void loop()
{
  sensor.check();

  while (sensor.available())
  {
    long irValue = sensor.getIR();
    long redValue = sensor.getRed();

    if (checkForBeat(irValue))
    {
      long delta = millis() - lastBeat;
      lastBeat = millis();

      bpm = 60.0 / (delta / 1000.0);

      if (bpm > 20 && bpm < 255)
      {
        rates[rateSpot++] = (byte)bpm;
        rateSpot %= RATE_SIZE;

        bpmMedia = 0;
        for (byte i = 0; i < RATE_SIZE; i++)
          bpmMedia += rates[i];

        bpmMedia /= RATE_SIZE;
      }
    }

    // Atualiza buffer
    for (byte i = 0; i < BUFFER_SIZE - 1; i++)
    {
      redBuffer[i] = redBuffer[i + 1];
      irBuffer[i] = irBuffer[i + 1];
    }

    redBuffer[BUFFER_SIZE - 1] = redValue;
    irBuffer[BUFFER_SIZE - 1] = irValue;

    calcularSpo2();

    Serial.println("--------------------------------");

    Serial.print("IR...........: ");
    Serial.println(irValue);

    Serial.print("RED..........: ");
    Serial.println(redValue);

    if (irValue < 50000)
    {
      Serial.println("Status.......: SEM DEDO");
    }
    else
    {
      Serial.println("Status.......: DEDO DETECTADO");
    }

    Serial.print("BPM..........: ");
    Serial.println(bpm, 1);

    Serial.print("BPM Medio....: ");
    Serial.println(bpmMedia);

    Serial.print("HeartRate Alg: ");
    if (validHeartRate)
      Serial.println(heartRateCalc);
    else
      Serial.println("--");

    Serial.print("SpO2.........: ");
    if (validSPO2)
    {
      Serial.print(spo2);
      Serial.println("%");
    }
    else
    {
      Serial.println("--");
    }

    sensor.nextSample();

    delay(50);
  }
}