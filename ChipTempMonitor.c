#include <stdio.h>
#include "pico/stdlib.h"

#define LEDV 13
#define BTNA 5
#define BTNB 6

void configPinos() {
    gpio_init(LEDV);
    gpio_set_dir(LEDV, GPIO_OUT);
    
    gpio_init(BTNA);
    gpio_set_dir(BTNA, GPIO_IN);
    gpio_pull_up(BTNA);

    
    gpio_init(BTNB);
    gpio_set_dir(BTNB, GPIO_IN);
    gpio_pull_up(BTNB);
}

int main()
{
    stdio_init_all();
    configPinos();

    while (true) {

        gpio_put(LEDV, 1);
        printf("Hello, world!\n");
        sleep_ms(100);
    }
}
