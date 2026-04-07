#ifndef UARTLOG_H
#define UARTLOG_H

#include "pico/stdlib.h"
#include "hardware/uart.h"

#define uartID uart0
#define uartTX 0
#define uartRX 1
#define uartRate 115200

void uartLogInit();

void uartLog(const char *msg);

void uartLogTemp(float temperatura, float limite, bool alerta);

#endif
