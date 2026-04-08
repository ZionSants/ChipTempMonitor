#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// Configuração do I2C
#define I2C_PORT i2c1
#define SDA 14 // Pino de dados
#define SCL 15 // Pino de clock 

// Qualquer arquivo com esse header pode acessar o display
extern ssd1306_t display;

// Define as telas disponíveis
typedef enum {
    telaTemperatura, // Tela principal
    telaGrafico // Tela do gráfico
} tela;

// Inicia o display
void displayInit();

// Atualiza o display com os valores de temperatura, limite, estado do buzzer e leitura do gráfico
void atualizarDisplay(float temperatura);
void analiseTemperatura(float temperatura);
void mostrarGrafico();
void graficoAdd(float temperatura);

#endif