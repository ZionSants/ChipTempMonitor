#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"

#define BUZZER 21 // Pino do buzzer

void buzzerInit();

void buzzerAlerta(uint freq, uint duration);

#endif
