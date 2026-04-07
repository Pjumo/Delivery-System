#include "Keypad.h"

static char keymap[4][4] = {{'1', '2', '3', 'A'},
                            {'4', '5', '6', 'B'},
                            {'7', '8', '9', 'C'},
                            {'*', '0', '#', 'D'}};

void keypad_init(void) {
    // Row(PB0~3) 출력 설정
    DDRB |= 0x0F;
    // Row 초기 HIGH 설정
    PORTB |= 0x0F;

    // Col(PD4~7) 입력 설정
    DDRD &= ~0xF0;
    // Col 내부 풀업 저항 활성화
    PORTD |= 0xF0;
}

static char last_key = 0;

char keypad_scan(void) {
    char current_key = 0;

    for (int r = 0; r < 4; r++) {
        // 모든 행을 HIGH로 만들어 초기화 (이전 스캔 잔류 제거)
        PORTB |= 0x0F; 
        
        // 현재 스캔할 행만 LOW로 설정
        PORTB &= ~(1 << r);
        
        // 안정화 시간
        _delay_us(100); 

        // 열 상태 읽기
        uint8_t col_state = (PIND & 0xF0); 
        
        if (col_state != 0xF0) { // 버튼이 눌린 경우
            for (int c = 0; c < 4; c++) {
                // 해당 열이 LOW인지 검사
                if (!(col_state & (1 << (c + 4)))) {
                    current_key = keymap[r][3-c];   // col 순서 반대
                    break;
                }
            }
        }
        
        // 행 스캔 직후 즉시 HIGH로 복구 (다음 행 간섭 방지)
        PORTB |= (1 << r);
        
        if (current_key) break;
    }

    // 키 중복 처리 및 노이즈 필터링
    if (current_key != last_key) {
        _delay_ms(30); // 디바운싱 시간
        last_key = current_key;
        
        // 키가 눌린 순간만 값을 반환
        if (current_key != 0) {
            return current_key;
        }
    }

    return 0;
}