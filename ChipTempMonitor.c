#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define I2C_PORT i2c1
#define SDA 14
#define SCL 15
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

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);

    ssd1306_t display;
    ssd1306_init(&display, 128, 64, 0x3C, I2C_PORT);


    while (true) {
        uint16_t valoradc = adc_read();

        float temperatura = conversao(valoradc); // Chama a função conversão para temperatura em Celsius

        char texto[20];
        sprintf(texto, "Temperatura: %.1f C", temperatura);

        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 20, 1, texto);
        ssd1306_show(&display);

        sleep_ms(1000);
    }
    return 0;
}
