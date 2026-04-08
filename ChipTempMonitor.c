#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "buzzer.h"
#include "display.h"
#include "sensores.h"
#include "uartLog.h"

#define BTNA 5 // Pino botão A
#define BTNB 6 // Pino botão B

volatile bool buzzerOn = true; // Controle do buzzer habilitado ou não
volatile bool statusAlterado = false; // Sinalização de pressão no botão
volatile bool lerTemperatura = false; // Controle do timer para atualizar temperatura

float ultimaTemp = 0.0f; // Última temperatura lida

repeating_timer_t timer;

// Callback do botão A para habilitar e desabilitar o buzzer
void botaoCallback(uint gpio, uint32_t events) {
    
    // Variáveis para debounce no botão A
    static uint32_t ultPressao = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());

    // Ignora o botão caso tenha sido pressionado há 200ms
    if(now - ultPressao < 200) return;
    ultPressao = now;

    if(gpio == BTNA) {
        buzzerOn = !buzzerOn;
        uartLog(buzzerOn ? "Alerta sonoro On" : "Alerta sonoro off");
    }
    statusAlterado = true; // Aviso para atualizar o display no loop principal
}

// Sinaliza para o loop principal realizar a leitura da temperatura
bool timerCallback(repeating_timer_t *rt) {
    lerTemperatura = true;
    return true;
}

int main()
{
    // Inicialização dos módulos
    stdio_init_all();
    uartLogInit();
    sensoresInit();
    buzzerInit();
    displayInit();

    #include "uartLog.h"

    // Inicialização dos botões
    gpio_init(BTNA);
    gpio_set_dir(BTNA, GPIO_IN);
    gpio_pull_up(BTNA);
 
    gpio_init(BTNB);
    gpio_set_dir(BTNB, GPIO_IN);
    gpio_pull_up(BTNB);
 
    gpio_set_irq_enabled_with_callback(BTNA, GPIO_IRQ_EDGE_FALL, true, &botaoCallback);

    // -1000 Utiliza intervalo fixo de leitura independente do callback
    add_repeating_timer_ms(-1000, timerCallback, NULL, &timer); 

    // Lê a temperatura e atualiza o display a cada 3 segundos
    while (true) {

        // Lê o joystick e altera o limite do alerta
        adc_select_input(0);
        uint16_t valorY = adc_read();
        ajustaLimite(valorY);

        // Atualiza o display caso o botão seja pressionado
        if(statusAlterado) {
            statusAlterado = false;
            analiseTemperatura(ultimaTemp);
        }

        // Atualiza o display caso o joystick saia da zona morta
        if(limiteAlterado) {
            limiteAlterado = false;
            analiseTemperatura(ultimaTemp);
        }

        // Quando o timer dispara, lê a temperatura e aciona o buzzer caso esteja no limite
        if(lerTemperatura) {
        lerTemperatura = false;
        adc_select_input(4);
        uint16_t valoradc = adc_read();
        ultimaTemp = conversao(valoradc); // Salva para usar no callback

        // Log a cada leitura
        uartLogTemp(ultimaTemp, limiteTemp, ultimaTemp >= limiteTemp);

            if(ultimaTemp >= limiteTemp && buzzerOn) {
                buzzerAlerta(523, 500);
            }
        analiseTemperatura(ultimaTemp);
        }
        sleep_ms(100);
    }
    return 0;

    // oi
}
