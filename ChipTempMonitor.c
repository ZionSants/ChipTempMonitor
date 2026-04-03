#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"

#define BUZZER 21 // Pino do buzzer

// Display
#define I2C_PORT i2c1
#define SDA 14
#define SCL 15
ssd1306_t display; // Declaração global do display

#define BTNA 5 // Pino do botão A
#define BTNB 6 // Pino do botão B
#define adcTemperatura 4 // Canal do sensor de temperatura interna

volatile bool buzzerOn = true; // Controle do buzzer habilitado ou não
float ultimaTemp = 0.0f; // Última temperatura lida
volatile bool statusAlterado = false; 

// Protótipos para função conseguir chamar a outra
void buzzerAlerta(uint freq, uint duration);
void analiseTemperatura(float temperatura);

// Callback do botão A para habilitar e desabilitar o buzzer
void botaoCallback(uint gpio, uint32_t events) {
    if(gpio == BTNA) {
        buzzerOn = !buzzerOn;
        statusAlterado = true; // Aviso para atualizar o display no loop principal
    }
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
    gpio_set_function(BUZZER, GPIO_FUNC_PWM); // Religa o controle do pino

    uint slice = pwm_gpio_to_slice_num(BUZZER);
    uint channel = pwm_gpio_to_channel(BUZZER);

    pwm_set_clkdiv(slice, 125.0f); // Divisor de clock

    //Determina a frequência do áudio
    uint32_t wrap = (1000000 / freq) - 1;
    pwm_set_wrap(slice, (uint16_t)wrap);

    pwm_set_chan_level(slice, channel, wrap / 2); // 50% duty cycle
    pwm_set_enabled(slice, true);
    sleep_ms(duration);
    pwm_set_enabled(slice, false);

    // Coloca o pino em low para evitar chiados
    gpio_set_function(BUZZER, GPIO_FUNC_SIO);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_put(BUZZER, 0);
}

// Análise da temperatura do sensor interno da placa
void analiseTemperatura(float temperatura) {
    char texto[25];
    ssd1306_clear(&display); // Limpa o framebuffer para escrever nova informação

    if (temperatura < 35.0f) {
        // Temperatura normal
        sprintf(texto, "Temperatura: %.1f C", temperatura);
        ssd1306_draw_string(&display, 0, 20, 1, texto);
    } else {
        //Temperatura alta, emite mensagem diferente e som
        sprintf(texto, "Alerta! > 35C");
        ssd1306_draw_string(&display, 0, 20, 1, texto);

        if (buzzerOn) {
            buzzerAlerta(523, 500);
        }
    }

    ssd1306_draw_string(&display, 0, 50, 1,
    buzzerOn ? "Alerta: ON " : "Alerta: OFF");
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

    // Lê a temperatura e atualiza o display a cada 3 segundos
    while (true) {

        // Atualiza o display caso o botão seja pressionado
        if(statusAlterado) {
            statusAlterado = false;
            analiseTemperatura(ultimaTemp);
        }

        uint16_t valoradc = adc_read();
        ultimaTemp = conversao(valoradc); // Salva para usar no callback
        analiseTemperatura(ultimaTemp);
        sleep_ms(3000);
    }
    return 0;
}
