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

// Bloco onde vamos armazenar os dados
byte bloco = 4;

// Variável que armazenará o texto digitado
String mensagem = "";

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
  Serial.println("      RFID RC522 + ESP32");
  Serial.println("==============================");

  Serial.println();
  Serial.println("Digite o valor que deseja gravar");
  Serial.println("e pressione ENTER:");
}

// -------------------------------
// LOOP
// -------------------------------

void loop() {

  // ---------------------------------
  // 1. VERIFICA SE O USUARIO DIGITOU
  // ---------------------------------

  if (Serial.available() > 0) {

    mensagem = Serial.readStringUntil('\n');

    // Remove espaços e quebras de linha
    mensagem.trim();

    if (mensagem.length() == 0) {

      Serial.println();
      Serial.println("Nenhum valor foi digitado.");
      Serial.println("Digite novamente:");

      return;
    }

    // Um bloco MIFARE possui somente 16 bytes
    if (mensagem.length() > 16) {

      Serial.println();
      Serial.println("ERRO: O valor possui mais de 16 caracteres.");
      Serial.println("Digite um valor com no maximo 16 caracteres:");

      mensagem = "";

      return;
    }

    Serial.println();
    Serial.println("------------------------------");
    Serial.print("Valor para gravar: ");
    Serial.println(mensagem);

    Serial.println();
    Serial.println("Aproxime o cartao RFID...");
  }

  // ---------------------------------
  // 2. SE NÃO TEM VALOR, NÃO FAZ NADA
  // ---------------------------------

  if (mensagem.length() == 0) {
    return;
  }

  // ---------------------------------
  // 3. VERIFICA SE EXISTE UM CARTÃO
  // ---------------------------------

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // ---------------------------------
  // 4. TENTA LER O CARTÃO
  // ---------------------------------

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println();
  Serial.println("==============================");
  Serial.println("      CARTAO DETECTADO!");
  Serial.println("==============================");

  // ---------------------------------
  // 5. MOSTRA UID
  // ---------------------------------

  Serial.print("UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }

  Serial.println();

  // ---------------------------------
  // 6. MOSTRA TIPO DO CARTÃO
  // ---------------------------------

  MFRC522::PICC_Type tipo;

  tipo = (MFRC522::PICC_Type)rfid.uid.sak;

  Serial.print("Tipo: ");
  Serial.println(rfid.PICC_GetTypeName(tipo));

  // ---------------------------------
  // 7. GRAVA O VALOR
  // ---------------------------------

  Serial.println();
  Serial.print("Gravando: ");
  Serial.println(mensagem);

  if (gravarBloco(bloco, mensagem)) {

    Serial.println();
    Serial.println("******************************");
    Serial.println(" DADOS GRAVADOS COM SUCESSO!");
    Serial.println("******************************");

  } else {

    Serial.println();
    Serial.println("ERRO AO GRAVAR OS DADOS!");
  }

  // ---------------------------------
  // 8. LÊ O VALOR GRAVADO
  // ---------------------------------

  Serial.println();
  Serial.println("Verificando dados gravados...");

  lerBloco(bloco);

  // ---------------------------------
  // 9. FINALIZA CARTÃO
  // ---------------------------------

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  // Limpa a mensagem
  mensagem = "";

  Serial.println();
  Serial.println("------------------------------");
  Serial.println("Digite outro valor:");
  Serial.println("------------------------------");
}

// =====================================================
// FUNÇÃO PARA GRAVAR DADOS
// =====================================================

bool gravarBloco(byte bloco, String texto) {

  MFRC522::StatusCode status;

  // Autenticação do bloco
  status = rfid.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A,
    bloco,
    &key,
    &(rfid.uid)
  );

  if (status != MFRC522::STATUS_OK) {

    Serial.print("Falha na autenticacao: ");
    Serial.println(rfid.GetStatusCodeName(status));

    return false;
  }

  // Buffer de 16 bytes
  byte dados[16];

  // Limpa o buffer
  memset(dados, 0, sizeof(dados));

  // Copia o texto para o buffer
  texto.getBytes(dados, sizeof(dados));

  // Grava no cartão
  status = rfid.MIFARE_Write(
    bloco,
    dados,
    16
  );

  if (status != MFRC522::STATUS_OK) {

    Serial.print("Falha na gravacao: ");
    Serial.println(rfid.GetStatusCodeName(status));

    return false;
  }

  return true;
}

// =====================================================
// FUNÇÃO PARA LER DADOS
// =====================================================

void lerBloco(byte bloco) {

  byte buffer[18];
  byte tamanho = sizeof(buffer);

  MFRC522::StatusCode status;

  // Autenticação
  status = rfid.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A,
    bloco,
    &key,
    &(rfid.uid)
  );

  if (status != MFRC522::STATUS_OK) {

    Serial.print("Falha na autenticacao: ");
    Serial.println(rfid.GetStatusCodeName(status));

    return;
  }

  // Leitura
  status = rfid.MIFARE_Read(
    bloco,
    buffer,
    &tamanho
  );

  if (status != MFRC522::STATUS_OK) {

    Serial.print("Falha na leitura: ");
    Serial.println(rfid.GetStatusCodeName(status));

    return;
  }

  Serial.print("Texto gravado: ");

  // Mostra os caracteres
  for (byte i = 0; i < 16; i++) {

    if (buffer[i] >= 32 && buffer[i] <= 126) {
      Serial.print((char)buffer[i]);
    }
  }

  Serial.println();

  // Mostra os bytes em hexadecimal
  Serial.print("HEX: ");

  for (byte i = 0; i < 16; i++) {

    if (buffer[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }

  Serial.println();
}