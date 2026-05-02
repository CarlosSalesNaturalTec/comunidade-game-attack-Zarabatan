# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**Attack Zarabatan** is the Attacker/Warrior firmware for a 4-player educational laser tag game (Comunidade GAME / Inclusão Digital). It runs on a NodeMCU ESP8266 and controls a laser weapon with stamina mechanics. MQTT is used for telemetry only — game logic runs entirely on-device to avoid network lag affecting gameplay.

## Build & Flash Commands

```bash
# Compile
platformio run

# Compile and flash to device
platformio run --target upload

# Open serial monitor (9600 baud)
platformio device monitor

# Compile + flash + monitor in one step
platformio run --target upload && platformio device monitor
```

Serial output is the primary debugging tool. No automated test suite exists; testing is manual via physical device.

## Architecture

### State Machine (`src/jogo_atacante.cpp`)

The core game loop is a 3-state machine:
- `JOGO_PAUSADO` → waiting for MQTT `START` command from Nexus
- `ARMA_PRONTA` → ready to fire (stamina > 0)
- `ARMA_SUPERAQUECIDA` → overheated, auto-recharging (stamina = 0)

State transitions happen in `jogo_verificarBotao()` (press events) and `jogo_verificarRecarga()` (timer-based). Both are called every `loop()` iteration — no blocking waits.

### Module Responsibilities

| File | Responsibility |
|------|---------------|
| `src/main.cpp` | `setup()` / `loop()` wiring only — no logic |
| `src/config.h` | All tunable constants (pins, timing, stamina, network) |
| `src/jogo_atacante.*` | State machine, button debounce, laser control, stamina |
| `src/mqtt_atacante.*` | Wi-Fi connection, MQTT pub/sub, offline fallback |
| `src/buzzer.*` | All audio feedback via `tone()` |

### MQTT Communication

The device connects to SSID `GAME_NEXUS` and broker at `192.168.1.100:1883`.

- **Publishes:** `game/atacante/stamina` → `{"stamina": 75, "status": "ativo"}`
- **Subscribes:** `game/nexus/comando` → `{"cmd": "START"}` triggers `jogo_iniciarPartida()`

The firmware handles offline gracefully — if Wi-Fi/broker is unreachable, button and laser still function. Reconnection is attempted every 5 seconds, non-blocking.

### Stamina System

- Max: 100%, cost per shot: 20% (5 shots per cycle)
- Overheat when stamina reaches 0 → 3-second auto-recharge timer
- Buzzer feedback intensity increases as stamina decreases (1 beep → 2 beeps → 3 beeps)

## Key Configuration (`src/config.h`)

All game-tunable parameters are in `config.h`. To deploy a second attacker unit, change `ID_MQTT` from `"atacante_01"` to `"atacante_02"`.

Hardware pins: Button=D1, Buzzer=D5, Laser=D6, optional LED=D7.

## Ecosystem Context

This firmware is one node in a 4-component system:
- **Atacante** (this project) — fires laser, tracks stamina
- **Defensor (Paladino)** — detects hits via LDR sensors, manages shield HP
- **Torre (Castle)** — the objective; destroyed when HP reaches 0
- **Nexus (Mestre)** — Python dashboard on a notebook; runs Mosquitto broker, sends START/STOP commands

The Defensor and Torre use LDR light sensors to detect laser hits locally, so MQTT is purely observational — the broker going down does not affect hit registration.
