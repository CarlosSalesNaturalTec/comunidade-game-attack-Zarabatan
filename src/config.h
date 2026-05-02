#pragma once

// =====================================================
// ARQUIVO DE CONFIGURAÇÃO — ATACANTE
// Ajuste aqui sem precisar mexer no resto do código!
// =====================================================

// ── Rede Wi-Fi ───────────────────────────────────────
#define SSID_DA_REDE       "GAME_NEXUS"
#define SENHA_DA_REDE      ""           // sem senha na rede do jogo

// ── Broker MQTT (notebook com IP fixo) ───────────────
#define IP_DO_BROKER       "192.168.1.100"
#define PORTA_MQTT         1883
#define ID_MQTT            "atacante_01"  // ID único — troque para "atacante_02" se tiver 2

// ── Tópicos MQTT ─────────────────────────────────────
#define TOPICO_STAMINA     "game/atacante/stamina"
#define TOPICO_COMANDO     "game/nexus/comando"

// ── Pinos do Hardware ─────────────────────────────────
//    NodeMCU v2 — veja o diagrama de ligação (WIRING.md)
#define PINO_BOTAO         D1   // GPIO5  — botão PBS-29 Arcade (INPUT_PULLUP)
#define PINO_BUZZER        D5   // GPIO14 — buzzer (+ direto, − no GND)
#define PINO_LASER         D6   // GPIO12 — módulo laser KY-008 pino S

// ── Opcional: LED interno do botão PBS-29 ─────────────
//    Requer resistor de 100Ω e circuito de latch — veja WIRING.md
//    Descomente para ativar o controle do LED do botão:
// #define PINO_LED_BOTAO   D7   // GPIO13 — ânodo do LED (cátodo no GND)

// ── Parâmetros do Jogo ────────────────────────────────
#define STAMINA_MAXIMA     100  // energia inicial em % (publicada no MQTT)
#define CUSTO_POR_TIRO     20   // % gasta por tiro — 20% = 5 tiros antes de superaquecer
#define TEMPO_RECARGA_MS   10000 // tempo de recarga: 10 segundos

// ── Debounce do Botão ─────────────────────────────────
//    Muito baixo → duplo acionamento | Muito alto → resposta lenta
#define DEBOUNCE_MS        20
