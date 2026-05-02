#pragma once
#include <Arduino.h>

// =====================================================
// MÓDULO DE JOGO — Lógica da Arma do Atacante
// =====================================================

// ── Estados possíveis da arma ─────────────────────────
enum EstadoArma {
  JOGO_PAUSADO,        // aguardando o comando START do Nexus
  ARMA_PRONTA,         // stamina > 0, pronta para atirar
  ARMA_SUPERAQUECIDA   // stamina = 0, recarregando
};

// ── Funções públicas (chamadas de main.cpp) ───────────
void jogo_iniciar();
void jogo_iniciarPartida();       // chamada pelo callback MQTT ao receber START

void jogo_verificarBotao();       // detecta press/release — chame em TODO loop()
void jogo_verificarRecarga();     // gerencia recarga automática — chame em TODO loop()

// ── Getters ───────────────────────────────────────────
int        jogo_getStamina();
EstadoArma jogo_getEstado();
