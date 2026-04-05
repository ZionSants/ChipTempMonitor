#ifndef SENSORES_H
#define SENSORES_H

#include "pico/stdlib.h"
#include "hardware/adc.h"

// Joystick
#define VRYPin 26 // Pino Y, Canal adc referente ao GPIO26
#define centroJoystick 2048 // Centro para que o código ignore o ruído do adc
#define zonaMortaJoystick 1500 // Faixa de valor pro sistema desconsiderar o joystick
#define modLeitura 0.5f // Quanto muda por leitura
// Limites mínimo e máximo de leitura
#define limiteMin 15.0f 
#define limiteMax 70.0f 

#define adcTemperatura 4 // Canal do sensor de temperatura interna

// Variáveis utilizadas em ChipTempMonitor e display
extern float limiteTemp;
extern volatile bool limiteAlterado;

// Inicializa ADC, sensor de temperatura interno e joystick
void sensoresInit();

// Função para ajustar o limite do alerta de temperatura
void ajustaLimite(uint16_t valorY);

// Função para ler o valor do adc e converter para Celsius
float conversao(uint16_t valoradc);

#endif