#pragma once
#include <Arduino.h>

// =====================================================
// MÓDULO MQTT — Comunicação com o Broker
// =====================================================

// ── Ciclo de vida ─────────────────────────────────────
void mqtt_iniciar();       // conecta Wi-Fi e broker (chame no setup())
void mqtt_reconectar();    // reconecta sem resetar o estado da arma
bool mqtt_conectado();
void mqtt_processar();     // processa mensagens recebidas (chame no loop())

// ── Publicação ────────────────────────────────────────
//    status: "ativo" | "superaquecido" | "recarregado"
void mqtt_publicarStamina(int stamina, const char* status);
