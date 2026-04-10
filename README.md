READ.ME
# 🌡️ ChipTempMonitor

Monitor de temperatura interna desenvolvido para a placa **BitDogLab (RP2040)**, com display OLED, alerta sonoro, ajuste interativo de limite via joystick e gráfico histórico de leituras.

---

## 📋 Funcionalidades

- **Leitura contínua** do sensor de temperatura interno do RP2040 (a cada 1 segundo)
- **Display OLED SSD1306** com duas telas alternáveis:
  - Tela principal: temperatura atual, limite configurado e estado do alerta
  - Tela de gráfico: histórico das últimas 10 leituras em barras
- **Alerta sonoro** via buzzer passivo quando a temperatura ultrapassa o limite
- **Limite ajustável** em tempo real pelo joystick (de 15°C a 70°C)
- **Controle do alerta** pelo botão A (liga/desliga o buzzer)
- **Alternância de tela** pelo botão B (temperatura ↔ gráfico)
- **Log serial** via USB — monitorável pelo PuTTY ou qualquer terminal serial

---

## 🔧 Hardware

| Componente | Detalhe |
|---|---|
| Placa | BitDogLab — RP2040 (Raspberry Pi Pico W) |
| Display | SSD1306 OLED 128x64 — I2C (SDA: GPIO 14, SCL: GPIO 15) |
| Buzzer | Passivo — GPIO 21 (PWM) |
| Joystick | Eixo Y analógico — GPIO 26 (ADC canal 0) |
| Botão A | GPIO 5 — habilita/desabilita alerta sonoro |
| Botão B | GPIO 6 — alterna entre tela de temperatura e gráfico |
| Sensor | Sensor de temperatura interno do RP2040 (ADC canal 4) |

---

## 🗂️ Estrutura do projeto

```
ChipTempMonitor/
├── ChipTempMonitor.c   # Ponto de entrada — loop principal, callbacks, timer
├── buzzer.c / .h       # Controle do buzzer via PWM (não bloqueante)
├── display.c / .h      # Display OLED — tela de temperatura e gráfico
├── sensores.c / .h     # ADC — leitura de temperatura e controle do joystick
├── uartLog.c / .h      # Log serial via USB
├── ssd1306.c / .h      # Biblioteca do display SSD1306
└── CMakeLists.txt      # Configuração de build (Pico SDK 2.2.0)
```

---

## ⚙️ Como compilar

### Pré-requisitos

- [Pico SDK 2.2.0](https://github.com/raspberrypi/pico-sdk)
- [VS Code](https://code.visualstudio.com/) com a extensão **Raspberry Pi Pico**
- CMake 3.13+
- ARM GCC Toolchain

### Build

```bash
mkdir build
cd build
cmake ..
make
```

O arquivo `ChipTempMonitor.uf2` será gerado na pasta `build`. Copie para a placa com o botão BOOTSEL pressionado.

---

## 🖥️ Log serial

Com a placa conectada via USB, abra um terminal serial (ex: PuTTY) na porta COM correspondente a **115200 baud**. A saída exibe:

```
Temp: 33.5 C | Limite: 35.0 C | Alerta: Não
Temp: 33.8 C | Limite: 35.0 C | Alerta: Não
[BTN] Alerta sonoro On
[JOY] Limite ajustado: 34.5 C
Temp: 35.1 C | Limite: 34.5 C | Alerta: Sim
```

---

## 🎮 Controles

| Controle | Ação |
|---|---|
| Joystick ↑ | Aumenta o limite de temperatura |
| Joystick ↓ | Diminui o limite de temperatura |
| Botão A | Liga ou desliga o alerta sonoro |
| Botão B | Alterna entre tela de temperatura e gráfico |

---

## 🏗️ Arquitetura do firmware

O firmware é organizado em torno de três eventos independentes, todos tratados no loop principal sem bloqueio:

```
Timer 1s         → lê temperatura, adiciona ao gráfico, aciona buzzer se necessário
Botão A/B (IRQ)  → alterna buzzer ou tela, redesenha display imediatamente  
Joystick (poll)  → ajusta limite, redesenha display em tempo real
```

O buzzer é **não bloqueante** — usa um `repeating_timer` interno que desliga o PWM após a duração definida, mantendo o loop livre durante o alerta.

---

## 📐 Tela de gráfico

```
Grafico          Alerta:ON
─────────────────────────────
 █               █
 █    █    █     █   █
 █    █    █  █  █   █  █
────────────────────────────
33   34   35  34  35  34  33
```

Exibe as últimas 10 leituras em barras proporcionais à escala de 15°C–70°C. Os valores inteiros aparecem abaixo de cada barra.

---

## 📦 Dependências

- [pico-ssd1306](https://github.com/daschr/pico-ssd1306) — biblioteca do display OLED (incluída no projeto)
- Pico SDK — `hardware_adc`, `hardware_i2c`, `hardware_pwm`, `hardware_timer`

---

## 👤 Autor

**ZionSants**  
Projeto desenvolvido como parte de estudos em sistemas embarcados com o RP2040.
