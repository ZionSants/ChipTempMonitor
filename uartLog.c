#include "uartLog.h"
#include <stdio.h>

void uartLogInit() {

}

void uartLog(const char *msg) {
    printf("%s\r\n", msg);
}

void uartLogTemp(float temperatura, float limite, bool alerta) {
    char buffer[60];
    snprintf(buffer, sizeof(buffer), "Temp: %.1f C | Limite: %.1f C | Alerta: %s",
    temperatura, limite, alerta ? "Sim" : "Não");
    uartLog(buffer);
}