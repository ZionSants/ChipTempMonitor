#include "buzzer.h"

static bool buzzerSom = false; // Controle para o buzzer não bloquear o joystick

static repeating_timer_t timerBuzzer;

// Callback para o timer do buzzer
bool buzzerCallback(repeating_timer_t *rt) {
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER), false);
    gpio_set_function(BUZZER, GPIO_FUNC_SIO);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_put(BUZZER, 0);
    buzzerSom = false;
    return false; 
}

void buzzerInit() {
    // Inicialização do buzzer
    gpio_set_function(BUZZER, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(BUZZER);
    pwm_set_enabled(slice, false);
}

// Função que emite um som no buzzer por um tempo
void buzzerAlerta(uint freq, uint duration) {
    if(buzzerSom) return;

    gpio_set_function(BUZZER, GPIO_FUNC_PWM); // Religa o controle do pino

    uint slice = pwm_gpio_to_slice_num(BUZZER);
    uint channel = pwm_gpio_to_channel(BUZZER);

    pwm_set_clkdiv(slice, 125.0f); // Divisor de clock

    //Determina a frequência do áudio
    uint32_t wrap = (1000000 / freq) - 1;
    pwm_set_wrap(slice, (uint16_t)wrap);

    pwm_set_chan_level(slice, channel, wrap / 2); // 50% duty cycle
    pwm_set_enabled(slice, true);
    
    buzzerSom = true;
    add_repeating_timer_ms(-duration, buzzerCallback, NULL, &timerBuzzer);
}

