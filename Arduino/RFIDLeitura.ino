/*
 */
#include <SPI.h>
#include <MFRC522.h>

/* DEFINIÇÃO DOS PINOS
| RC522        | ESP32 DevKit V1 | Função              |
| ------------ | --------------: | ------------------- |
| **SDA / SS** |      GPIO **5** | Chip Select         |
| **SCK**      |     GPIO **18** | Clock SPI           |
| **MOSI**     |     GPIO **23** | Dados ESP32 → RC522 |
| **MISO**     |     GPIO **19** | Dados RC522 → ESP32 |
| **IRQ**      |    Não conectar | Interrupção         |
| **GND**      |             GND | Terra               |
| **RST**      |     GPIO **22** | Reset               |
| **3.3V**     |         **3V3** | Alimentação         |
*/

#define SS_PIN   5
#define RST_PIN  22

MFRC522 rfid(SS_PIN, RST_PIN);

// Chave padrão dos cartões MIFARE
MFRC522::MIFARE_Key key;

// Bloco onde os dados foram gravados
byte bloco = 4;


// -------------------------------
// SETUP
// -------------------------------

void setup() {

  Serial.begin(115200);

  // Inicializa SPI
  SPI.begin();

  // Inicializa RC522
  rfid.PCD_Init();

  // Configura chave padrão
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println();
  Serial.println("==============================");
  Serial.println("      LEITOR RFID + ESP32");
  Serial.println("==============================");
  Serial.println();
  Serial.println("Aproxime um cartao RFID...");
}


// -------------------------------
// LOOP
// -------------------------------

void loop() {

  // Verifica se existe um novo cartão
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Tenta ler o cartão
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println();
  Serial.println("==============================");
  Serial.println("      CARTAO DETECTADO!");
  Serial.println("==============================");


  // -------------------------------
  // MOSTRA UID
  // -------------------------------

  Serial.print("UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }

  Serial.println();


  // -------------------------------
  // MOSTRA TIPO DO CARTAO
  // -------------------------------

  MFRC522::PICC_Type tipo;

  tipo = (MFRC522::PICC_Type)rfid.uid.sak;

  Serial.print("Tipo: ");
  Serial.println(rfid.PICC_GetTypeName(tipo));


  // -------------------------------
  // LÊ OS DADOS
  // -------------------------------

  Serial.println();
  Serial.println("Dados armazenados:");

  String dadoCartao = lerBloco(bloco);

  if (dadoCartao != "") {

    Serial.print("Enviando para API: ");
    Serial.println(dadoCartao);
  
  }


  // -------------------------------
  // FINALIZA CARTAO
  // -------------------------------

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  Serial.println();
  Serial.println("------------------------------");
  Serial.println("Aproxime outro cartao...");
  Serial.println("------------------------------");

  delay(2000);
}


// =====================================================
// FUNÇÃO PARA LER DADOS DO CARTÃO
// =====================================================

String lerBloco(byte bloco) {

  byte buffer[18];
  byte tamanho = sizeof(buffer);

  MFRC522::StatusCode status;

  // -------------------------------
  // AUTENTICAÇÃO
  // -------------------------------

  status = rfid.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A,
    bloco,
    &key,
    &(rfid.uid)
  );

  if (status != MFRC522::STATUS_OK) {

    Serial.print("Erro na autenticacao: ");
    Serial.println(rfid.GetStatusCodeName(status));

    return "";
  }

  // -------------------------------
  // LEITURA DO BLOCO
  // -------------------------------

  status = rfid.MIFARE_Read(
    bloco,
    buffer,
    &tamanho
  );

  if (status != MFRC522::STATUS_OK) {

    Serial.print("Erro na leitura: ");
    Serial.println(rfid.GetStatusCodeName(status));

    return "";
  }

  // -------------------------------
  // TRANSFORMA OS BYTES EM STRING
  // -------------------------------

  String dadoCartao = "";

  for (byte i = 0; i < 16; i++) {

    if (buffer[i] >= 32 && buffer[i] <= 126) {

      dadoCartao += (char)buffer[i];
    }
  }

  // -------------------------------
  // MOSTRA O VALOR
  // -------------------------------

  Serial.print("Valor: ");
  Serial.println(dadoCartao);

  // Retorna o valor lido
  return dadoCartao;
}