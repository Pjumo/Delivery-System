#include "IR_Receiver.h"

static volatile uint32_t ir_data = 0;
static volatile uint8_t  ir_ready = 0;
static volatile uint32_t temp_code = 0;
static volatile uint8_t  bit_count = 0;

void ir_init(void) {
    // PD2(INT0) 입력 설정
    DDRD &= ~(1 << PD2);
    // PD2 풀업 활성화
    PORTD |= (1 << PD2);

    // 인터럽트 설정 (기존 코드와 동일)
    EICRA |= (1 << ISC01); // Falling Edge
    EIMSK |= (1 << INT0);  // Enable INT0
    
    // 타이머 설정 (기존 코드와 동일)
    TCCR1B |= (1 << CS11) | (1 << CS10); 
    sei();
}

uint8_t ir_available(void) {
    return ir_ready;
}

uint32_t ir_get_command(void) {
    ir_ready = 0; // 플래그 리셋
    return ir_data;
}

// 외부 인터럽트 서비스 루틴: 펄스 사이의 시간을 측정하여 비트 판단
ISR(INT0_vect) {
    uint16_t duration = TCNT1; // 이전 인터럽트 이후 경과 시간 확인
    TCNT1 = 0; // 타이머 초기화

    // NEC 프로토콜 타이밍 (Prescaler 64 기준, 1틱 = 4us)
    // Lead code: 약 13.5ms (3375 틱)
    if (duration > 3000 && duration < 3500) {
        bit_count = 0;
        temp_code = 0;
        return;
    }

    if (bit_count < 32) {
        // Logical '1': 약 2.25ms (562 틱)
        // Logical '0': 약 1.12ms (280 틱)
        if (duration > 400 && duration < 700) {
            temp_code |= (1UL << (31 - bit_count));
        }
        
        bit_count++;
        
        if (bit_count == 32) {
            ir_data = temp_code;
            ir_ready = 1;
        }
    }
}