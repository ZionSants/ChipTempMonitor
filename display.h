#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// Display
#define I2C_PORT i2c1
#define SDA 14
#define SCL 15

// Qualquer arquivo com esse header pode acessar o display
extern ssd1306_t display;

// Inicia o display
void displayInit();

// Atualiza o display com os valores de temperatura, o limite e o estado do buzzer
void analiseTemperatura(float temperatura);

#endif