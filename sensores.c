#include "sensores.h"
#include "uartLog.h"
#include <stdio.h>

float limiteTemp = 35.0f; // Limite padrão ajustável de temperatura
volatile bool limiteAlterado = false; // Controle do limite de temperatura em tempo real

void sensoresInit() {
    // Inicialização do módulo adc
    adc_init();
    adc_gpio_init(VRYPin); // Pino gp26 para leitura analógica do ADC
    adc_set_temp_sensor_enabled(true); // Sensor de temperatura interna habilitado
    adc_select_input(adcTemperatura);
}

// Função para ajustar o limite do alerta de temperatura
void ajustaLimite(uint16_t valorY) {
    if(valorY < centroJoystick - zonaMortaJoystick) {
        limiteTemp -= modLeitura;
        // Trava o limite no mínimo
        if(limiteTemp < limiteMin) {
            limiteTemp = limiteMin;
        }
        limiteAlterado = true;
    }else if(valorY > centroJoystick + zonaMortaJoystick) {
        limiteTemp += modLeitura;
        // Trava o limite no máximo
        if(limiteTemp > limiteMax) {
            limiteTemp = limiteMax;
        }
        limiteAlterado = true;
    }
    if(limiteAlterado) {
        char buffer[40];
        snprintf(buffer, sizeof(buffer), "[JOY] Limite ajustado: %.1f C", limiteTemp);
        uartLog(buffer);
    }
}

// Função para converter o valor do adc para graus Celsius
float conversao(uint16_t valoradc) {
    const float bitsToVolts = 3.3f / (1 << 12); // Fator de conversão de bits para volts
    float voltagem = valoradc * bitsToVolts; // Conversão do adc para volts
    float temperatura = 27.0f - (voltagem - 0.706f) / 0.001721f; // Calculo da temperatura
    return temperatura;
}