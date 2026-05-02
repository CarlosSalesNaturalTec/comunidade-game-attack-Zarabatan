#include "jogo_atacante.h"
#include "buzzer.h"
#include "mqtt_atacante.h"
#include "config.h"

// ── Variáveis internas de estado ──────────────────────
static EstadoArma    _estado          = JOGO_PAUSADO;
static int           _stamina         = STAMINA_MAXIMA;
static unsigned long _inicioRecargaMs = 0;

// ── Debounce do botão ──
static bool          _estadoAnterior  = HIGH;  // HIGH = solto (pull-up)
static bool          _estadoEstavel   = HIGH;
static unsigned long _ultimoDebounceMs = 0;

// ─────────────────────────────────────────────────────
// FUNÇÕES INTERNAS (privadas)
// ─────────────────────────────────────────────────────

static void laserLigar() {
  digitalWrite(PINO_LASER, HIGH);
  #ifdef PINO_LED_BOTAO
    digitalWrite(PINO_LED_BOTAO, HIGH);
  #endif
}

static void laserDesligar() {
  digitalWrite(PINO_LASER, LOW);
  #ifdef PINO_LED_BOTAO
    digitalWrite(PINO_LED_BOTAO, LOW);
  #endif
  buzzer_silencio();
}

// Realiza um tiro: liga laser, debita stamina, publica e verifica superaquecimento
static void atirar() {
  _stamina -= CUSTO_POR_TIRO;
  if (_stamina < 0) _stamina = 0;

  laserLigar();
  buzzer_tiro(_stamina);          // som varia conforme stamina restante

  Serial.print("[JOGO] TIRO! Stamina: ");
  Serial.print(_stamina);
  Serial.print("% / ");
  Serial.print(STAMINA_MAXIMA);
  Serial.println("%");

  mqtt_publicarStamina(_stamina, "ativo");

  if (_stamina <= 0) {
    // Arma esgotada → superaquecimento imediato
    laserDesligar();
    _estado          = ARMA_SUPERAQUECIDA;
    _inicioRecargaMs = millis();

    Serial.println("[JOGO] *** Arma SUPERAQUECIDA! Recarregando... ***");
    mqtt_publicarStamina(0, "superaquecido");
    buzzer_superaquecimento();    // alarme sonoro de superaquecimento
  }
}

// ─────────────────────────────────────────────────────
// FUNÇÕES PÚBLICAS
// ─────────────────────────────────────────────────────

void jogo_iniciar() {
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  pinMode(PINO_LASER, OUTPUT);
  digitalWrite(PINO_LASER, LOW);

  #ifdef PINO_LED_BOTAO
    pinMode(PINO_LED_BOTAO, OUTPUT);
    digitalWrite(PINO_LED_BOTAO, LOW);
  #endif

  _estado   = JOGO_PAUSADO;
  _stamina  = STAMINA_MAXIMA;

  Serial.println("[JOGO] Atacante inicializado. Aguardando START...");
}

void jogo_iniciarPartida() {
  _stamina         = STAMINA_MAXIMA;
  _estado          = ARMA_PRONTA;
  _estadoEstavel   = HIGH;
  _estadoAnterior  = HIGH;
  _inicioRecargaMs = 0;

  laserDesligar();

  Serial.println("[JOGO] *** PARTIDA INICIADA! Arma pronta para atirar. ***");
  mqtt_publicarStamina(_stamina, "ativo");
}

// Lê o botão PBS-29 com debounce e aciona a lógica de jogo
void jogo_verificarBotao() {
  bool leitura = digitalRead(PINO_BOTAO);

  // ── Reinicia o contador de debounce ao detectar qualquer mudança ──
  if (leitura != _estadoAnterior) {
    _ultimoDebounceMs = millis();
  }

  // ── Processa apenas após o sinal estabilizar ──────────────────────
  if ((millis() - _ultimoDebounceMs) >= DEBOUNCE_MS) {

    if (leitura != _estadoEstavel) {
      _estadoEstavel = leitura;

      if (_estadoEstavel == LOW) {
        // ────── BOTÃO PRESSIONADO (borda de descida) ──────────────

        if (_estado == ARMA_PRONTA) {
          // Arma pronta → atirar!
          atirar();

        } else if (_estado == ARMA_SUPERAQUECIDA) {
          // Arma travada → avisa sonoramente que não pode atirar
          Serial.println("[JOGO] Arma superaquecida! Aguarde a recarga.");
          buzzer_superaquecimento();

        } else {
          // Jogo pausado → ignora o botão
          Serial.println("[JOGO] Jogo pausado. Aguardando START...");
        }

      } else {
        // ────── BOTÃO SOLTO (borda de subida) ────────────────────
        // Desliga o laser e o buzzer ao soltar
        // (só tem efeito se a arma estava em ARMA_PRONTA)
        if (_estado == ARMA_PRONTA) {
          laserDesligar();
          Serial.println("[JOGO] Botao solto — laser OFF.");
        }
      }
    }
  }

  _estadoAnterior = leitura;
}

// Verifica se o tempo de recarga da arma terminou — chame em TODO loop()
void jogo_verificarRecarga() {
  if (_estado != ARMA_SUPERAQUECIDA) return;

  if ((millis() - _inicioRecargaMs) >= TEMPO_RECARGA_MS) {
    // Recarga completa!
    _stamina = STAMINA_MAXIMA;
    _estado  = ARMA_PRONTA;

    Serial.println("[JOGO] Arma RECARREGADA! Pronta para atirar.");
    mqtt_publicarStamina(_stamina, "ativo");
    buzzer_pronto();   // melodia de "pronto para atirar"
  }
}

int jogo_getStamina() {
  return _stamina;
}

EstadoArma jogo_getEstado() {
  return _estado;
}
