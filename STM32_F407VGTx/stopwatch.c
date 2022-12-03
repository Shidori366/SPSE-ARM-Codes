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
char savedMsArr[2], savedSArr[2], savedMArr[2];

OS_TID taskID1_btn;
OS_TID taskID2_time;
OS_TID taskID3_lcd;

void btnService() {
    while (1) {
        if (io_read(USER_BUTTON)) {
            saved = 1;
            sprintf(savedMArr, "%d", minutes);
            sprintf(savedSArr, "%d", seconds);
            sprintf(savedMsArr, "%d ", ms);
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
        char msArr[8], sArr[2], mArr[2];

        LCD_set(LCD_CUR_OFF);
        sprintf(mArr, "%d", minutes);
        sprintf(sArr, "%d", seconds);
        sprintf(msArr, "%d ", ms);

        LCD_set(LCD_LINE1);
        LCD_print(mArr);
        LCD_print(":");
        LCD_print(sArr);
        LCD_print(":");
        LCD_print(msArr);

        if (saved) {
            LCD_set(LCD_LINE2);
            LCD_print(savedMArr);
            LCD_print(":");
            LCD_print(savedSArr);
            LCD_print(":");
            LCD_print(savedMsArr);
        }

        memset(msArr, 0, sizeof(mArr));
        memset(msArr, 0, sizeof(sArr));
        memset(msArr, 0, sizeof(msArr));
    }
}

__task void board_init(void) {
    LED_setup();
    LCD_setup();

    taskID1_btn = os_tsk_create(btnService, 0);
    taskID2_time = os_tsk_create(timeService, 0);
    taskID3_lcd = os_tsk_create(lcdService, 0);

    os_tsk_delete_self();
}

int main() {
    os_sys_init(board_init);
}
