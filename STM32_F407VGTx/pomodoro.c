#include <RTL.h>
#include "stm32_kit.h"
#include "stm32_kit/lcd.h"
#include "stm32_kit/led.h"

#include <stdbool.h>
#include <stdio.h>

OS_TID taskID1_countdown;
OS_TID taskID2_printing;
OS_TID taskID3_skipping;

const int work_time = 25;
const int short_break_time = 5;
const int long_break_time = 15;
const int max_work_count = 4;

int work_count = 0;
bool work_r = true;
bool counting = false;
bool start = false;

int current_mins = 0;
int current_secs = 0;

__task void countdown(void) {
    while(true) {
        if (!start) {
            continue;
        }

        // Setting countdown time
        if (!counting) {
            if (work_r) {
                current_mins = work_time;
            } else {
                current_mins = work_count < max_work_count ? short_break_time : long_break_time;
                work_count = work_count == max_work_count ? 0 : work_count;
            }
            current_secs = 0;
            counting = true;
        }

        // Counting
        delay_ms(1000);
        if(current_secs == 0) {
            --current_mins;
            current_secs = 59;
        } else {
            --current_secs;
        }

        // After countdown ends
        if (current_mins == 0 && current_secs == 0) {
            if (work_r) {
                ++work_count;
            }
            work_r = !work_r;
            counting = false;
        }
    }
}

__task void printing(void) {
    char time_arr[6];

    while(true) {
        snprintf(time_arr, sizeof(time_arr), "%02d:%02d", current_mins, current_secs);

        LCD_set(LCD_LINE1);
        LCD_print(time_arr);

        LCD_set(LCD_LINE2);
        if (work_r) {
            LCD_print("WORKING!");
        } else {
            LCD_print("RESTING!");
        }
    }
}

__task void skip_or_start(void) {
    while(true) {
        if(io_read(USER_BUTTON) && counting) {
            if(start) {
                if(work_r) {
                    ++work_count;
                }

                work_r = !work_r;
                counting = false;
            } else {
                start = true;
            }
            delay_ms(1000);
        }
    }
}

__task void board_init(void) {
    LCD_setup();
    LED_setup();
    LCD_set(LCD_CUR_OFF);
    
    taskID1_countdown = os_tsk_create(countdown, 0);
    taskID2_printing = os_tsk_create(printing, 0);
    taskID3_skipping = os_tsk_create(skip_or_start, 0);

    os_tsk_delete_self (); // Delete the init(self) task
}

int main(void) {
    os_sys_init(board_init);
}
