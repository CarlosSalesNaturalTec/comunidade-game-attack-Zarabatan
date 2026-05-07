#include "mqtt_atacante.h"
#include "jogo_atacante.h"
#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ── Objetos de rede (privados) ────────────────────────
static WiFiClient   _wifi;
static PubSubClient _mqtt(_wifi);

// ─────────────────────────────────────────────────────
// CALLBACK: chamado automaticamente ao receber mensagem
// ─────────────────────────────────────────────────────
static void aoReceberMensagem(char* topico, byte* payload, unsigned int tamanho) {
  char mensagem[tamanho + 1];
  memcpy(mensagem, payload, tamanho);
  mensagem[tamanho] = '\0';

  Serial.print("[MQTT] Mensagem em '");
  Serial.print(topico);
  Serial.print("': ");
  Serial.println(mensagem);

  if (String(topico) == TOPICO_COMANDO) {
    StaticJsonDocument<64> doc;
    if (deserializeJson(doc, mensagem) != DeserializationError::Ok) return;

    const char* cmd = doc["cmd"] | "";

    if (strcmp(cmd, "START") == 0) {
      Serial.println("[MQTT] → Comando START recebido!");
      jogo_iniciarPartida();
    }
    // Futuros comandos (PAUSE, STOP, RESET) podem ser adicionados aqui
  }
}

// ─────────────────────────────────────────────────────
void mqtt_iniciar() {
  // ── 1. Configurar broker e callback (independe do Wi-Fi) ──
  _mqtt.setServer(IP_DO_BROKER, PORTA_MQTT);
  _mqtt.setCallback(aoReceberMensagem);

  // ── 2. Tentar conectar ao Wi-Fi com timeout ────────
  //    Se a rede não estiver disponível em 10s, a arma
  //    entra em MODO OFFLINE: botão e laser funcionam
  //    normalmente. O loop() tentará reconectar depois.
  Serial.print("[MQTT] Conectando ao Wi-Fi '");
  Serial.print(SSID_DA_REDE);
  Serial.print("' (timeout: 10s)");

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_DA_REDE, SENHA_DA_REDE);

  constexpr unsigned long TIMEOUT_WIFI_MS = 5000; // 5 segundos
  unsigned long inicio = millis();

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - inicio >= TIMEOUT_WIFI_MS) {
      Serial.println();
      Serial.println("[MQTT] Wi-Fi nao encontrado — MODO OFFLINE ativado.");
      Serial.println("[MQTT] Partida sera iniciada automaticamente.");
      Serial.println("[MQTT] Reconexao automatica sera tentada no loop().");
      digitalWrite(LED_BUILTIN, LOW);  // reacende: setup ainda em andamento
      return;
    }
    delay(500);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // pisca: "buscando Wi-Fi"
    Serial.print(".");
  }
  digitalWrite(LED_BUILTIN, LOW);   // sólido: Wi-Fi conectado

  Serial.print(" OK! IP: ");
  Serial.println(WiFi.localIP());

  // ── 3. Conectar ao broker MQTT ────────────────────
  mqtt_reconectar();
}

void mqtt_reconectar() {
  if (_mqtt.connected()) return;

  // ── Sem Wi-Fi → tenta reconectar a rede primeiro ──
  //    Usa uma única tentativa não-bloqueante (sem while).
  //    O loop() chamará mqtt_reconectar() de novo no próximo ciclo.
  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long _ultimaTentativaWifi = 0;
    if (millis() - _ultimaTentativaWifi < 5000) return; // espera 5s entre tentativas
    _ultimaTentativaWifi = millis();

    Serial.println("[MQTT] Wi-Fi desconectado. Tentando reconectar...");
    WiFi.disconnect();
    WiFi.begin(SSID_DA_REDE, SENHA_DA_REDE);
    return;  // retorna — não trava o loop()
  }

  // ── Wi-Fi OK → tenta o broker MQTT (máx 1 vez a cada 5s) ───────────────
  static unsigned long _ultimaTentativaMqtt = 0;
  if (millis() - _ultimaTentativaMqtt < 5000) return;
  _ultimaTentativaMqtt = millis();

  Serial.print("[MQTT] Conectando ao broker " IP_DO_BROKER ":" );
  Serial.print(PORTA_MQTT);
  Serial.print("...");

  if (_mqtt.connect(ID_MQTT)) {
    Serial.println(" Conectado!");
    _mqtt.subscribe(TOPICO_COMANDO);
    Serial.print("[MQTT] Assinando: ");
    Serial.println(TOPICO_COMANDO);

    // Republicar stamina atual ao reconectar — mantém o Nexus sincronizado
    const char* status = (jogo_getEstado() == ARMA_SUPERAQUECIDA)
                         ? "superaquecido" : "ativo";
    mqtt_publicarStamina(jogo_getStamina(), status);

  } else {
    Serial.print(" Falhou! Codigo MQTT: ");
    Serial.println(_mqtt.state());
  }
}

bool mqtt_conectado() {
  return _mqtt.connected();
}

void mqtt_processar() {
  _mqtt.loop();
}

// ─────────────────────────────────────────────────────
void mqtt_publicarStamina(int stamina, const char* status) {
  // Monta: {"stamina": 80, "status": "ativo"}
  StaticJsonDocument<96> doc;
  doc["stamina"] = stamina;
  doc["status"]  = status;

  char payload[96];
  serializeJson(doc, payload);

  _mqtt.publish(TOPICO_STAMINA, payload);

  Serial.print("[MQTT] Publicou stamina: ");
  Serial.println(payload);
}
