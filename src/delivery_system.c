#include "IR_Receiver.h"
#include "Keypad.h"
#include "UART0.h"
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define LED_PIN PB5

// 시스템 상태 정의
typedef enum {
    STATE_WAIT_DELIVERY, // 택배 대기 (LED OFF)
    STATE_LOCKED         // 택배 보관 중 (LED ON)
} SystemState;

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
SystemState current_state = STATE_WAIT_DELIVERY;

char password[5] = ""; // 설정된 비밀번호
char input_buffer[5] = ""; // 입력 임시 저장
uint8_t input_idx = 0;

// IR 리모컨 커맨드를 숫자로 변환하는 함수
char ir_to_num(uint8_t cmd) {
    switch (cmd) {
    case 0x68:
        return '0';
    case 0x30:
        return '1';
    case 0x18:
        return '2';
    case 0x7A:
        return '3';
    case 0x10:
        return '4';
    case 0x38:
        return '5';
    case 0x5A:
        return '6';
    case 0x42:
        return '7';
    case 0x4A:
        return '8';
    case 0x52:
        return '9';
    default:
        return 0;
    }
}

void reset_input() {
    memset(input_buffer, 0, sizeof(input_buffer));
    input_idx = 0;
}

void reset_pwd() {
    memset(password, 0, sizeof(password));
    input_idx = 0;
}

int main(void) {
    UART0_init();
    stdout = &OUTPUT;
    keypad_init();
    ir_init();

    DDRB |= (1 << LED_PIN);   // LED 출력 설정
    PORTB &= ~(1 << LED_PIN); // 초기 OFF

    printf("Smart Delivery System Ready...\r\n");

    while (1) {
        // 키패드 입력 처리 (택배 기사)
        char kp_key = keypad_scan();
        if (kp_key != 0 && current_state == STATE_WAIT_DELIVERY) {
            if (kp_key >= '0' && kp_key <= '9' && input_idx < 4) {
                password[input_idx++] = kp_key;
                printf("%c", kp_key); // 키패드 입력 표시
            } else if (kp_key == '#') {
                if (input_idx == 4) {
                    current_state = STATE_LOCKED;
                    PORTB |= (1 << LED_PIN); // 택배 보관 표시 (LED ON)
                    printf("\r\n[Delivery Completed] Door Locked.\r\n");
                    input_idx = 0;
                } else {
                    printf("\r\n[Error] Password Must Be 4-Digit!\r\n");
                    reset_pwd();
                }
            }
        }

        // IR 리모컨 입력 처리 (수령인)
        if (ir_available() && current_state == STATE_LOCKED) {
            uint32_t raw_data = ir_get_command();
            uint8_t cmd =
                (uint8_t)((raw_data >> 8) & 0xFF); // Command 바트 추출

            char ir_key = ir_to_num(cmd);

            if (ir_key != 0 && input_idx < 4) {
                input_buffer[input_idx++] = ir_key;
                printf("%c", ir_key); // 리모컨 입력 표시
            } else if (cmd == 0x90) { // EQUAL 버튼 (확인)
                if (strcmp(input_buffer, password) == 0) {
                    current_state = STATE_WAIT_DELIVERY;
                    PORTB &= ~(1 << LED_PIN); // 수령 완료 (LED OFF)
                    printf("\r\n[Pickup Completed] Door Unlocked.\r\n");
                } else {
                    printf("\r\n[Error] Auth Failed!\r\n");
                }
                reset_input();
            }
        }
    }
}