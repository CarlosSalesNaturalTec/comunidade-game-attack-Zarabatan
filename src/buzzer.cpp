#include "buzzer.h"

// ─────────────────────────────────────────────────────
// Função auxiliar: toca um bip e pausa
// ─────────────────────────────────────────────────────
static void bip(int frequencia, int duracaoMs, int pausaMs = 0) {
  tone(PINO_BUZZER, frequencia, duracaoMs);
  delay(duracaoMs);
  if (pausaMs > 0) {
    noTone(PINO_BUZZER);
    delay(pausaMs);
  }
}

// ─────────────────────────────────────────────────────
void buzzer_iniciar() {
  pinMode(PINO_BUZZER, OUTPUT);
  noTone(PINO_BUZZER);
  Serial.println("[BUZZER] Inicializado.");
}

// ─────────────────────────────────────────────────────
// Som ao disparar — o número de bips avisa a stamina restante:
//
//   stamina > 40%  → 1 bip  (normal)
//   stamina 21–40% → 2 bips (aviso)
//   stamina 1–20%  → 3 bips (crítico — próximo tiro pode superaquecer!)
// ─────────────────────────────────────────────────────
void buzzer_tiro(int stamina) {
  if (stamina > 40) {
    // ── Normal: 1 bip grave ────────────────────────
    bip(1200, 75);

  } else if (stamina > 20) {
    // ── Aviso: 2 bips médios ───────────────────────
    bip(1500, 55, 40);
    bip(1500, 55);

  } else {
    // ── Crítico: 3 bips agudos urgentes ──────────
    bip(2200, 45, 30);
    bip(2200, 45, 30);
    bip(2200, 45);
  }
}

// ─────────────────────────────────────────────────────
// Para o som imediatamente (ao soltar o botão)
// ─────────────────────────────────────────────────────
void buzzer_silencio() {
  noTone(PINO_BUZZER);
}

// ─────────────────────────────────────────────────────
// Alarme de superaquecimento — arma travada!
// Bloqueante (~450ms) — ok: arma já está em estado SUPERAQUECIDA
// ─────────────────────────────────────────────────────
void buzzer_superaquecimento() {
  // Sequência descendente: transmite a sensação de "energia esgotada"
  bip(1800, 80, 30);
  bip(1400, 80, 30);
  bip(1000, 80, 30);
  bip(700,  100);
  noTone(PINO_BUZZER);
}

// ─────────────────────────────────────────────────────
// Melodia de recarga completa — arma pronta de novo!
// Bloqueante (~420ms) — ok: fim do estado SUPERAQUECIDA
// ─────────────────────────────────────────────────────
void buzzer_pronto() {
  // Sequência ascendente: transmite a sensação de "energia restaurada"
  bip(523, 100, 30);   // Dó
  bip(659, 100, 30);   // Mi
  bip(784, 130);       // Sol
  noTone(PINO_BUZZER);
}
