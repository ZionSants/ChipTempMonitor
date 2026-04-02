#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <string.h>

#define LEDV 13 // Pino do led vermelho
#define BTNA 5 // Pino do botão A
#define BTNB 6 // Pino do botão B
#define adcTemperatura 4 // Canal do sensor de temperatura interna

// Função para configurar os pinos utilizados
void configPinos() {
    // Inicialização do led
    gpio_init(LEDV);
    gpio_set_dir(LEDV, GPIO_OUT);
    
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

    while (true) {
        uint16_t valoradc = adc_read();

        float temperatura = conversao(valoradc); // Chama a função conversão para temperatura em Celsius

        printf("Temperatura atual: %.2f °C\n", temperatura);

        sleep_ms(500);
    }
    return 0;
}
