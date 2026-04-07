#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "Keypad.h"
#include "IR_Receiver.h"
#include "UART0.h"
#include "i2c.h"
#include "lcd1602_i2c.h"
#include "buzzer.h"
#include "servo.h"

typedef enum {
    STATE_WAIT_DELIVERY,
    STATE_LOCKED
} SystemState;

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
SystemState current_state = STATE_WAIT_DELIVERY;

char password[5] = ""; 
char input_buffer[5] = ""; 
uint8_t input_idx = 0;

// LCD 초기 화면 및 공통 문구 함수화
void display_standby() {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_print("Box empty...");
}

void display_locked() {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_print("Package in store"); // 16자 제한 고려
    lcd_gotoxy(0, 1);
    lcd_print("PWD: ");
    lcd_print(password);
}

char ir_to_num(uint8_t cmd) {
    switch (cmd) {
        case 0x68: return '0'; case 0x30: return '1'; case 0x18: return '2';
        case 0x7A: return '3'; case 0x10: return '4'; case 0x38: return '5';
        case 0x5A: return '6'; case 0x42: return '7'; case 0x4A: return '8';
        case 0x52: return '9';
        default: return 0;
    }
}

void reset_input() {
    memset(input_buffer, 0, sizeof(input_buffer));
    input_idx = 0;
}

int main(void) {
    // 1. 모든 모듈 초기화
    UART0_init();
    stdout = &OUTPUT;
    i2c_init();
    lcd_init();
    keypad_init();
    ir_init();
    buzzer_init();
    servo_init();

    // 초기 상태 표시
    display_standby();
    printf("Smart Delivery System Ready...\r\n");

    while (1) {
        // --- [택배 기사: 키패드 입력] ---
        char kp_key = keypad_scan();
        if (kp_key != 0 && current_state == STATE_WAIT_DELIVERY) {
            if (kp_key >= '0' && kp_key <= '9' && input_idx < 4) {
                password[input_idx++] = kp_key;
                lcd_gotoxy(input_idx - 1, 1); // 두 번째 줄에 입력값 표시
                lcd_data('*');
                printf("%c", kp_key);
            } 
            else if (kp_key == '#') {
                if (input_idx == 4) {
                    current_state = STATE_LOCKED;
                    display_locked(); // LCD 상태 변경

                    play_delivery_done();

                    printf("\r\n[Locked] PWD: %s\r\n", password);
                    input_idx = 0;
                } else {
                    lcd_gotoxy(0, 1);
                    lcd_print("Must be 4-digit!");
                    _delay_ms(1000);
                    display_standby();
                    memset(password, 0, sizeof(password));
                    input_idx = 0;
                }
            }
        }

        // --- [수령인: IR 리모컨 입력] ---
        if (ir_available() && current_state == STATE_LOCKED) {
            uint32_t raw_data = ir_get_command();
            uint8_t cmd = (uint8_t)((raw_data >> 8) & 0xFF);
            char ir_key = ir_to_num(cmd);

            if (ir_key != 0 && input_idx < 4) {
                input_buffer[input_idx++] = ir_key;
                lcd_gotoxy(input_idx + 4, 1); // 두 번째 줄에 입력값 표시
                lcd_data('*');
                printf("%c", ir_key);
            } 
            else if (cmd == 0x90) { // EQUAL 버튼
                if (strcmp(input_buffer, password) == 0) {
                    current_state = STATE_WAIT_DELIVERY;

                    servo_write(180);
                    
                    // 수령 완료 메시지
                    lcd_clear();
                    lcd_gotoxy(0, 0);
                    lcd_print("Receipt completed!");
                    printf("\r\n[Success] Receipt completed!\r\n");
                    
                    _delay_ms(2000); // 2초 대기

                    servo_write(0);
                    display_standby(); // 다시 대기 상태로
                    memset(password, 0, sizeof(password));
                } else {
                    lcd_gotoxy(5, 1);
                    lcd_print("Error!    "); // 기존 비번 가리기
                    printf("\r\n");
                    _delay_ms(1000);
                    display_locked(); // 다시 비번 입력 대기 화면
                }
                reset_input();
            }
        }
    }
}