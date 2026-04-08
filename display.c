#include "display.h"
#include "buzzer.h"
#include <stdio.h>

// Configuração da página de gráfico (Aperte botão B)
#define graphColumn 10 // Número de colunas visíveis no gráfico
#define graphColumnWidth 11 // Largura da coluna
#define graphGap 2 // Espaço entre as colunas
#define graphText 10 // Altura para os valores abaixo das colunas
#define graphBar (64 - graphText) // Altura que as colunas podem utilizar (54 pixels)
#define tempMin 15.0f // Temperatura mínima da escala do gráfico
#define tempMax 70.0f // Temperatura máxima " " "

// Variáveis externas
extern float limiteTemp; // Limite ajustável pelo joystick
extern volatile bool buzzerOn; // Estado do buzzer On/Off
extern volatile tela telaAtual; // Tela principal ou de gráfico ativa

ssd1306_t display; // Declaração global do display

// Buffer de temperaturas, armazena as últimas leituras de coluna do gráfico
static float historico[graphColumn] = {0};
static uint8_t indice = 0;

// Adiciona uma nova leitura ao gráfico 
void graficoAdd(float temperatura) {
    historico[indice] = temperatura;
    indice = (indice + 1) % graphColumn; // Volta ao início
}

// Renderiza o gráfico no framebuffer do display
void graficoDraw(float limiteTemp) {
    // Título, status do buzzer e linha abaixo
    ssd1306_draw_string(&display, 0, 0, 1, "Grafico");
    ssd1306_draw_string(&display, 70, 0, 1, buzzerOn ? "Alerta:ON " : "Alerta:OFF");
    for(int x = 0; x < 128; x++) {
        ssd1306_draw_pixel(&display, x, 9);
    }

    // Desenha as colunas do gráfico
    for(int i = 0; i < graphColumn; i++) {
        // Ordenação do mais antigo pro mais recente
        int pos = (indice + i) % graphColumn;
        float temp = historico[pos];

        int xInit = i * (graphColumnWidth + graphGap); // Posição inicial

        // Ignora colunas sem dados
        if(temp == 0.0f) continue;

        // Mapeamento da temperatura e altura
        float normalizado = (temp - tempMin) / (tempMax - tempMin);
        if(normalizado < 0.0f) normalizado = 0.0f;
        if(normalizado > 1.0f) normalizado = 1.0f;
        int altura = (int)(normalizado * graphBar);
        if(altura < 1) altura = 1;

        // Desenha a coluna de baixo pra cima
        int yBase = 63 - graphText;
        for(int px = 0; px < graphColumnWidth; px++) {
            for(int py = 0; py < altura; py++) {
                ssd1306_draw_pixel(&display, xInit + px, yBase - py);
            }
        }
        // Indica o valor da temperatura abaixo da coluna
        char valor[5];
        sprintf(valor, "%d", (int)temp);
        // Centraliza o texto em cada coluna
        int xText = xInit + (graphColumnWidth / 2) - 3;
        ssd1306_draw_string(&display, xText, 64 - graphText + 1, 1, valor);
    }
}

// Inicialização do display
void displayInit() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA, GPIO_FUNC_I2C);
    gpio_set_function(SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SDA);
    gpio_pull_up(SCL);

    ssd1306_init(&display, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&display);
    ssd1306_show(&display);
}

// Decide qual tela irá aparecer (Botão B)
void atualizarDisplay(float temperatura) {
    if(telaAtual == telaTemperatura) {
        analiseTemperatura(temperatura);
    }else {
        mostrarGrafico();
    }
}

// Análise da temperatura do sensor interno da placa
void analiseTemperatura(float temperatura) {
    char texto1[25];
    char texto2[25];
    char limTexto[25];

    ssd1306_clear(&display); // Limpa o framebuffer para escrever nova informação

    if (temperatura < limiteTemp) {
        // Temperatura normal
        sprintf(texto1, "Temperatura: %.1f C", temperatura);
        ssd1306_draw_string(&display, 0, 10, 1, texto1);
    } else {
        //Temperatura alta, emite mensagem de alerta em duas linhas
        sprintf(texto1, "Alerta! %.1f C", temperatura);
        ssd1306_draw_string(&display, 17, 10, 1, texto1);
        sprintf(texto2, "Temperatura excedida");
        ssd1306_draw_string(&display, 3, 20, 1, texto2);
    }
    sprintf(limTexto, "Limite C: %.1f C", limiteTemp);
    ssd1306_draw_string(&display, 0, 35, 1, limTexto);
    ssd1306_draw_string(&display, 0, 50, 1, buzzerOn ? "Alerta: ON " : "Alerta: OFF");

    ssd1306_show(&display); // Envia o framebuffer pro display
}

// Limpa o display, renderiza o gráfico e envia pro display
void mostrarGrafico() {
    ssd1306_clear(&display);
    graficoDraw(limiteTemp);
    ssd1306_show(&display);
}