#ifndef UARTLOG_H
#define UARTLOG_H

#include "pico/stdlib.h"
#include "hardware/uart.h"

// Configuração do UART
#define uartID uart0
#define uartTX 0 // Pino de transmissão
#define uartRX 1 // Pino de recepção
#define uartRate 115200 // Baudrate padrão

void uartLogInit();

void uartLog(const char *msg); // Envia uma mensagem pela serial

void uartLogTemp(float temperatura, float limite, bool alerta); // Formata e envia os dados

#endif
