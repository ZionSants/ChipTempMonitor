#include "uartLog.h"
#include <stdio.h>

// Comunicação foi feita por usb portanto, não foi necessário configurar aqui
void uartLogInit() {
}

// Envia uma mensagem pela interface serial
void uartLog(const char *msg) {
    printf("%s\r\n", msg);
}

// Formata os dados que irão ser enviados
void uartLogTemp(float temperatura, float limite, bool alerta) {
    char buffer[60];
    snprintf(buffer, sizeof(buffer), "Temp: %.1f C | Limite: %.1f C | Alerta: %s",
    temperatura, limite, alerta ? "Sim" : "Não");
    uartLog(buffer);
}