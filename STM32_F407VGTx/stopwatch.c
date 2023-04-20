// LCD
#define LCD_COLS    8
#define LCD_ROWS    2

#include <RTL.h>
#include <stdio.h>
#include "stm32_kit.h"
#include "stm32_kit/led.h"
#include "stm32_kit/lcd.h"
#include <string.h>

int ms = 0, seconds = 0, minutes = 0, saved = 0;
char savedTimeArr[9];

OS_TID taskID1_btn;
OS_TID taskID2_time;
OS_TID taskID3_lcd;

void btnService() {
    while (1) {
        if (io_read(USER_BUTTON)) {
            snprintf(savedTimeArr, sizeof(savedTimeArr), "%02d:%02d:%02d", minutes, seconds, ms);
            saved = 1;
        }
    }
}

void timeService() {
    while (1) {
        delay_ms(100);
        ++ms;

        if (ms == 10) {
            ++seconds;
            ms = 0;
        }

        if (seconds == 60) {
            ++minutes;
            seconds = 0;
        }
    }
}

void lcdService() {
    while (1) {
        char timeArr[9];

        snprintf(timeArr, sizeof(timeArr),"%02d:%02d:%02d", minutes, seconds, ms);

        LCD_set(LCD_LINE1);
        LCD_print(timeArr);

        if (saved) {
            LCD_set(LCD_LINE2);
            LCD_print(savedTimeArr);
        }

        memset(timeArr, 0, sizeof(timeArr));
        memset(savedTimeArr, 0, sizeof(savedTimeArr));
    }
}

__task void board_init(void) {
    LED_setup();
    LCD_setup();
    LCD_set(LCD_CUR_OFF);

    taskID1_btn = os_tsk_create(btnService, 0);
    taskID2_time = os_tsk_create(timeService, 0);
    taskID3_lcd = os_tsk_create(lcdService, 0);

    os_tsk_delete_self();
}

int main() {
    os_sys_init(board_init);
}
