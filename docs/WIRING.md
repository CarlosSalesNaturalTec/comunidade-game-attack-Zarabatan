# Ligação Elétrica — Atacante

## Diagrama de Conexões

```
NodeMCU v2              Botão Arcade PBS-29
─────────────           ────────────────────
D1  (GPIO5)  ──────────  Terminal A  (sinal — INPUT_PULLUP interno)
GND          ──────────  Terminal B  (referência)

NodeMCU v2              Módulo Laser KY-008 (6mm 650nm 5mW)
─────────────           ──────────────────────────────────────
Vin (5V)     ──────────  VCC  ← 5V recomendado para potência total
                               (3V3 funciona mas o laser fica mais fraco)
GND          ──────────  GND
D6  (GPIO12) ──────────  S   (pino de sinal — HIGH = laser ligado)

NodeMCU v2              Buzzer
─────────────           ──────────────────
D5  (GPIO14) ──────────  (+) ânodo / pino positivo
GND          ──────────  (−) cátodo / pino negativo
```

### Observações sobre o Buzzer

**Buzzer ATIVO** (com adesivo na parte inferior): o `tone()` funciona
corretamente — o buzzer bipa no ritmo do sinal mesmo com o oscilador interno.
Recomendado para facilidade de uso.

**Buzzer PASSIVO** (sem adesivo): também compatível com `tone()`.
As frequências serão audíveis (Dó, Mi, Sol na melodia de recarga).

---

## LED Interno do Botão PBS-29 (opcional)

O PBS-29 possui LED interno para iluminação. A ligação direta ao GPIO
do NodeMCU pode não fornecer corrente suficiente. Use o circuito abaixo:

```
NodeMCU v2
─────────────
Vin (5V) ──────────────────┐
                           [resistor 100Ω]
                           │
D7 (GPIO13) ──[NPN 2N2222 base via 1kΩ]── coleta
                           │               ├── LED (+) do botão
                          GND              └── LED (−) do botão → GND
```

Para ativar no firmware:
1. Descomente `#define PINO_LED_BOTAO D7` em `config.h`.
2. O LED do botão acenderá junto com o laser ao atirar.

---

## Tabela Resumo de Pinos

| Componente      | Pino NodeMCU | GPIO   | Modo       |
|-----------------|-------------|--------|------------|
| Botão PBS-29 A  | D1          | GPIO5  | INPUT_PULLUP |
| Botão PBS-29 B  | GND         | —      | —          |
| Laser KY-008 S  | D6          | GPIO12 | OUTPUT     |
| Laser KY-008 +  | Vin (5V)    | —      | —          |
| Laser KY-008 −  | GND         | —      | —          |
| Buzzer (+)      | D5          | GPIO14 | OUTPUT     |
| Buzzer (−)      | GND         | —      | —          |
| LED Botão (opt) | D7          | GPIO13 | OUTPUT     |

---

## Teste Rápido sem Broker MQTT

O firmware aceita upload sem o Mosquitto ativo.
O loop de reconexão imprime falhas no Serial Monitor (9600 baud)
mas **não trava** o sistema — botão e laser funcionam normalmente.

Para testar a eletrônica:
1. Grave o firmware via PlatformIO.
2. Abra o Serial Monitor (9600).
3. Pressione o botão → "TIRO! Stamina: 80%" deve aparecer.
4. Após 5 tiros → "Arma SUPERAQUECIDA!" e alarme sonoro.
5. Após 3s → "Arma RECARREGADA!" e melodia de pronto.
