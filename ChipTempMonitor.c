#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"
#include "hardware/timer.h"

#define BUZZER 21 // Pino do buzzer

// Centro para que o código ignore o ruído do adc
#define centroJoystick 2048 
#define zonaMortaJoystick 1500 // Faixa de valor pro sistema desconsiderar o joystick

#define modLeitura 0.5f // Quanto muda por leitura

// Limites mínimo e máximo de leitura
#define limiteMin 15.0f 
#define limiteMax 70.0f 

#define VRYPin 26 // Pino Y, Canal adc referente ao GPIO26

// Display
#define I2C_PORT i2c1
#define SDA 14
#define SCL 15
ssd1306_t display; // Declaração global do display

#define BTNA 5 // Pino do botão A
#define BTNB 6 // Pino do botão B
#define adcTemperatura 4 // Canal do sensor de temperatura interna

volatile bool buzzerOn = true; // Controle do buzzer habilitado ou não
volatile bool statusAlterado = false; 
volatile bool lerTemperatura = false; // Controle do timer para atualizar temperatura
volatile bool limiteAlterado = false; // Controle do limite de temperatura em tempo real

static bool buzzerSom = false; // Controle para o buzzer não bloquear o joystick

static repeating_timer_t timerBuzzer;
repeating_timer_t timer;

float ultimaTemp = 0.0f; // Última temperatura lida
float limiteTemp = 35.0f; // Limite padrão ajustável de temperatura

// Protótipos para função conseguir chamar a outra
void buzzerAlerta(uint freq, uint duration);
void analiseTemperatura(float temperatura);

void ajustaLimite(uint16_t valorY) {
    if(valorY < centroJoystick - zonaMortaJoystick) {
        limiteTemp -= modLeitura;
        if(limiteTemp < limiteMin) {
            limiteTemp = limiteMin;
        }
        limiteAlterado = true;
    }else if(valorY > centroJoystick + zonaMortaJoystick) {
        limiteTemp += modLeitura;
        if(limiteTemp > limiteMax) {
            limiteTemp = limiteMax;
        }
        limiteAlterado = true;
    }
}

// Callback do botão A para habilitar e desabilitar o buzzer
void botaoCallback(uint gpio, uint32_t events) {
    if(gpio == BTNA) {
        buzzerOn = !buzzerOn;
    }
    statusAlterado = true; // Aviso para atualizar o display no loop principal
}

bool timerCallback(repeating_timer_t *rt) {
    lerTemperatura = true;
    return true;
}

bool buzzerCallback(repeating_timer_t *rt) {
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER), false);
    gpio_set_function(BUZZER, GPIO_FUNC_SIO);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_put(BUZZER, 0);
    buzzerSom = false;
    return false; 
}

// Função para configurar os pinos utilizados
void configPinos() { 
    // Inicialização dos botões
    gpio_init(BTNA);
    gpio_set_dir(BTNA, GPIO_IN);
    gpio_pull_up(BTNA);
    
    gpio_init(BTNB);
    gpio_set_dir(BTNB, GPIO_IN);
    gpio_pull_up(BTNB);

    // Inicialização do módulo adc
    adc_init();
    adc_gpio_init(VRYPin); // Pino gp26 para leitura analógica do ADC
    adc_set_temp_sensor_enabled(true);
    adc_select_input(adcTemperatura);

    // Inicialização do display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);

    // Inicialização do buzzer
    gpio_set_function(BUZZER, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(BUZZER);
    pwm_set_enabled(slice, false);

    gpio_set_irq_enabled_with_callback(BTNA, GPIO_IRQ_EDGE_FALL, true, &botaoCallback);
}

// Função que emite um som no buzzer por um tempo
void buzzerAlerta(uint freq, uint duration) {
    if(buzzerSom) return;

    gpio_set_function(BUZZER, GPIO_FUNC_PWM); // Religa o controle do pino

    uint slice = pwm_gpio_to_slice_num(BUZZER);
    uint channel = pwm_gpio_to_channel(BUZZER);

    pwm_set_clkdiv(slice, 125.0f); // Divisor de clock

    //Determina a frequência do áudio
    uint32_t wrap = (1000000 / freq) - 1;
    pwm_set_wrap(slice, (uint16_t)wrap);

    pwm_set_chan_level(slice, channel, wrap / 2); // 50% duty cycle
    pwm_set_enabled(slice, true);
    
    buzzerSom = true;
    add_repeating_timer_ms(-duration, buzzerCallback, NULL, &timerBuzzer);
}

// Análise da temperatura do sensor interno da placa
void analiseTemperatura(float temperatura) {
    char texto1[25];
    char texto2[25];
    char limTexto[25];

    ssd1306_clear(&display); // Limpa o framebuffer para escrever nova informação

    if (temperatura < limiteTemp) {
        // Temperatura normal
        sprintf(texto1, "Temperatura: %.1f C", temperatura);
        ssd1306_draw_string(&display, 0, 10, 1, texto1);
    } else {
        //Temperatura alta, emite mensagem diferente e som
        sprintf(texto1, "Alerta! %.1f C", temperatura);
        ssd1306_draw_string(&display, 17, 10, 1, texto1);
        sprintf(texto2, "Temperatura excedida");
        ssd1306_draw_string(&display, 3, 20, 1, texto2);
    }
    sprintf(limTexto, "Limite C: %.1f C", limiteTemp);
    ssd1306_draw_string(&display, 0, 35, 1, limTexto);
    ssd1306_draw_string(&display, 0, 50, 1, buzzerOn ? "Alerta: ON " : "Alerta: OFF");
    ssd1306_show(&display); // Envia o framebuffer pro display
}

// Função para converter o valor do adc para graus Celsius
float conversao(uint16_t valoradc) {
    const float bitsToVolts = 3.3f / (1 << 12); // Fator de conversão de bits para volts
    float voltagem = valoradc * bitsToVolts; // Conversão do adc para volts
    float temperatura = 27.0f - (voltagem - 0.706f) / 0.001721f; // Calculo da temperatura
    return temperatura;
}

int main()
{
    stdio_init_all();
    configPinos();

    // Inicialização do display via I2C
    ssd1306_init(&display, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&display);
    ssd1306_show(&display);

    add_repeating_timer_ms(-1000, timerCallback, NULL, &timer); // -3000 Utiliza intervalo fixo de leitura independente do callback

    // Lê a temperatura e atualiza o display a cada 3 segundos
    while (true) {

        adc_select_input(0);
        uint16_t valorY = adc_read();
        ajustaLimite(valorY);

        // Atualiza o display caso o botão seja pressionado
        if(statusAlterado) {
            statusAlterado = false;
            analiseTemperatura(ultimaTemp);
        }

        // Atualiza o display caso o joystick saia da zona morta
        if(limiteAlterado) {
            limiteAlterado = false;
            analiseTemperatura(ultimaTemp);
        }

        // Quando o timer dispara, lê a temperatura
        if(lerTemperatura) {
        lerTemperatura = false;
        adc_select_input(4);
        uint16_t valoradc = adc_read();
        ultimaTemp = conversao(valoradc); // Salva para usar no callback

            if(ultimaTemp >= limiteTemp && buzzerOn) {
                buzzerAlerta(523, 500);
            }
        analiseTemperatura(ultimaTemp);
        }
        sleep_ms(100);
    }
    return 0;
}
