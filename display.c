#include "display.h"
#include "buzzer.h"
#include <stdio.h>

// Variáveis externas para exibir o valor correto no display
extern float limiteTemp;
extern volatile bool buzzerOn;

ssd1306_t display; // Declaração global do display

// Inicialização do display
void displayInit() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);

    ssd1306_init(&display, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&display);
    ssd1306_show(&display);
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
        //Temperatura alta, emite mensagem de alerta em duas linhas
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