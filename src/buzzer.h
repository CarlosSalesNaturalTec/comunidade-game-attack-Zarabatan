#pragma once
#include <Arduino.h>
#include "config.h"

// =====================================================
// MÓDULO DE SOM — Buzzer da Arma
//
// Cada situação de jogo tem um som diferente:
//   tiro normal  → 1 bip curto
//   stamina baixa (≤ 40%)  → 2 bips rápidos
//   stamina crítica (≤ 20%) → 3 bips urgentes
//   superaquecimento        → alarme descendente
//   recarga completa        → melodia ascendente
//
// Compatível com buzzer ATIVO (5V) e PASSIVO (tone).
// =====================================================

void buzzer_iniciar();

// Sons de jogo — chamados pelo módulo jogo_atacante
void buzzer_tiro(int stamina);     // som varia conforme a stamina restante
void buzzer_silencio();             // corta qualquer som em andamento
void buzzer_superaquecimento();     // alarme ao travar a arma (bloqueante ~450ms)
void buzzer_pronto();               // melodia de recarga completa (bloqueante ~420ms)
