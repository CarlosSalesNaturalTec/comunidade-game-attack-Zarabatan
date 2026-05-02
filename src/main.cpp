#include <Arduino.h>
#include "config.h"
#include "buzzer.h"
#include "mqtt_atacante.h"
#include "jogo_atacante.h"

// ==========================================================
// Firmware do artefato: Atack Zarabatan
// Projeto: Comunidade Game - Inclusão Digital 
//
// Hardware: NodeMCU ESP8266 v2 (CH340G)
//           Botão Arcade PBS-29 (D1 — INPUT_PULLUP)
//           Módulo Laser 6mm 650nm 5mW   (D6)
//           Buzzer ativo ou passivo       (D5)
//
// Descrição da Arma:
//   O Atacante pressiona o botão → laser dispara + bip sonoro.
//   Cada tiro gasta 20% de stamina (5 tiros no total).
//   A intensidade do bip avisa a stamina restante:
//     > 40%   → 1 bip  (normal)
//     21–40%  → 2 bips (aviso)
//     1–20%   → 3 bips (crítico!)
//   Com stamina 0% → arma SUPERAQUECE: alarme + laser bloqueado.
//   Após 3 segundos → recarga automática + melodia de pronto.
//   A stamina é publicada via MQTT a cada tiro e a cada recarga.
// ==========================================================

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("=================================================");
  Serial.println("   Arma: Atack Zarabatan - v1.0");
  Serial.println("   Comunidade Game - Projeto de Inclusão Digital");
  Serial.println("=================================================");

  buzzer_iniciar();   // configura o pino e testa o buzzer
  mqtt_iniciar();     // conecta ao Wi-Fi e ao broker MQTT
  jogo_iniciar();     // configura botão, laser e estado inicial

  // Modo offline: sem broker disponível, inicia a partida imediatamente
  if (!mqtt_conectado()) {
    jogo_iniciarPartida();
    Serial.println("[SETUP] Pronto! (modo offline — arma ativa)");
  } else {
    Serial.println("[SETUP] Pronto! Aguardando START do Nexus...");
  }
  Serial.println();
}

void loop() {
  // ── Passo 1: manter a conexão com o broker ────────────
  //    Se cair durante a partida, reconecta sem resetar a stamina
  if (!mqtt_conectado()) {
    mqtt_reconectar();
  }
  mqtt_processar();   // processa mensagens MQTT recebidas

  // ── Passo 2: verificar o botão e disparar ────────────
  jogo_verificarBotao();

  // ── Passo 3: verificar se a recarga terminou ─────────
  jogo_verificarRecarga();
}
