#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "tft_lcd_ili9341/ili9341/ili9341.h"
#include "tft_lcd_ili9341/gfx/gfx_ili9341.h"
#include "tft_lcd_ili9341/touch_resistive/touch_resistive.h"
#include "image_bitmap.h"

#define SCREEN_ROTATION 1
#define MOTOR_IN1 5
#define MOTOR_IN2 4
#define MOTOR_IN3 3
#define MOTOR_IN4 2

#define IMG_X ((320 - 51) / 2)
#define IMG_Y 20
#define BTN_Y 140
#define BTN_H 60

// Centro do golfinho
#define CX (IMG_X + 25)
#define CY (IMG_Y + 32)
#define RADIUS 40

// 8 posições da seta ao redor do golfinho
const int arrow_dx[8] = { 0,  28,  40,  28,   0, -28, -40, -28};
const int arrow_dy[8] = {-40, -28,   0,  28,  40,  28,   0, -28};

void motor_init() {
    gpio_init(MOTOR_IN1); gpio_set_dir(MOTOR_IN1, GPIO_OUT);
    gpio_init(MOTOR_IN2); gpio_set_dir(MOTOR_IN2, GPIO_OUT);
    gpio_init(MOTOR_IN3); gpio_set_dir(MOTOR_IN3, GPIO_OUT);
    gpio_init(MOTOR_IN4); gpio_set_dir(MOTOR_IN4, GPIO_OUT);
}

void motor_step_cw() {
    gpio_put(MOTOR_IN4, 1); sleep_ms(5); gpio_put(MOTOR_IN4, 0);
    gpio_put(MOTOR_IN3, 1); sleep_ms(5); gpio_put(MOTOR_IN3, 0);
    gpio_put(MOTOR_IN2, 1); sleep_ms(5); gpio_put(MOTOR_IN2, 0);
    gpio_put(MOTOR_IN1, 1); sleep_ms(5); gpio_put(MOTOR_IN1, 0);
    sleep_ms(5);
}

void motor_step_ccw() {
    gpio_put(MOTOR_IN1, 1); sleep_ms(5); gpio_put(MOTOR_IN1, 0);
    gpio_put(MOTOR_IN2, 1); sleep_ms(5); gpio_put(MOTOR_IN2, 0);
    gpio_put(MOTOR_IN3, 1); sleep_ms(5); gpio_put(MOTOR_IN3, 0);
    gpio_put(MOTOR_IN4, 1); sleep_ms(5); gpio_put(MOTOR_IN4, 0);
    sleep_ms(5);
}

void motor_stop() {
    gpio_put(MOTOR_IN1, 0); gpio_put(MOTOR_IN2, 0);
    gpio_put(MOTOR_IN3, 0); gpio_put(MOTOR_IN4, 0);
}

void draw_dolphin(uint16_t color) {
    gfx_drawBitmap(IMG_X, IMG_Y, image_NFC_dolphin_emulation_bits, 51, 64, color);
}

void clear_arrow(int pos) {
    int x = CX + arrow_dx[pos] - 5;
    int y = CY + arrow_dy[pos] - 5;
    gfx_fillRect(x, y, 10, 10, 0x0000);
}

void draw_arrow(int pos, uint16_t color) {
    int x = CX + arrow_dx[pos];
    int y = CY + arrow_dy[pos];
    gfx_fillRect(x - 4, y - 4, 8, 8, color);
}

int main(void) {
    stdio_init_all();
    motor_init();

    LCD_initDisplay();
    LCD_setRotation(SCREEN_ROTATION);
    gfx_init();
    gfx_clear();
    configure_touch();

    draw_dolphin(0xFFFF);

    GFX_Button btn_cw  = {10,  BTN_Y, 140, BTN_H};
    GFX_Button btn_ccw = {170, BTN_Y, 140, BTN_H};

    gfx_But_drawRect(&btn_cw,  0x07E0);
    gfx_But_drawRect(&btn_ccw, 0x001F);

    gfx_setTextSize(1);
    gfx_setTextColor(0xFFFF);
    gfx_drawText(40,  BTN_Y + 25, "HORARIO");
    gfx_drawText(195, BTN_Y + 25, "ANTI-HOR");

    int px, py;
    int state = 0;
    int last_state = 0;
    int arrow_pos = 0;
    int anim_tick = 0;

    while (true) {
        if (readPoint(&px, &py)) {
            int sx, sy;
            gfx_touchTransform(SCREEN_ROTATION, px, py, &sx, &sy);
            if (gfx_But_isPressed(&btn_cw, sx, sy))       state = -1;
            else if (gfx_But_isPressed(&btn_ccw, sx, sy)) state = 1;
        } else {
            state = 0;
        }

        // Limpa seta quando para
        if (state == 0 && last_state != 0) {
            clear_arrow(arrow_pos);
            draw_dolphin(0xFFFF);
        }

        if (state == 1) {
            motor_step_ccw();
            anim_tick++;
            if (anim_tick % 8 == 0) {
                clear_arrow(arrow_pos);
                arrow_pos = (arrow_pos + 1) % 8;
                draw_arrow(arrow_pos, 0x001F); // azul girando CW
            }
        } else if (state == -1) {
            motor_step_cw();
            anim_tick++;
            if (anim_tick % 8 == 0) {
                clear_arrow(arrow_pos);
                arrow_pos = (arrow_pos + 7) % 8; // sentido contrário
                draw_arrow(arrow_pos, 0x07E0); // verde girando CCW
            }
        } else {
            motor_stop();
        }

        last_state = state;
    }
}