#include "Buzzer.h"

static volatile uint16_t toggle_count = 0;

void buzzer_init(void) {
    BUZZER_DDR |= (1 << BUZZER_PIN); // PB4 출력 설정
    
    // Timer2 설정: CTC 모드
    TCCR2A = (1 << WGM21); 
    TCCR2B = (1 << CS22) | (1 << CS21); // Prescaler 256
    
    // 인터럽트 비활성 상태로 시작
    TIMSK2 &= ~(1 << OCIE2A);
}

void buzzer_beep(uint16_t frequency, uint16_t duration_ms) {
    if (frequency == 0) return;

    // OCR2A 계산 (주파수의 절반 주기로 인터럽트 발생)
    // 인터럽트가 두 번 발생해야 한 주기가 완성되므로 2를 더 곱해줌
    OCR2A = (F_CPU / (2 * 256 * frequency)) - 1;
    
    TIMSK2 |= (1 << OCIE2A); // 인터럽트 활성화 (소리 시작)

    // 가독성을 위한 지연 (이 동안 ISR이 핀을 흔들어줌)
    for (uint16_t i = 0; i < duration_ms; i++) {
        _delay_ms(1);
    }

    TIMSK2 &= ~(1 << OCIE2A); // 인터럽트 비활성화 (소리 끄기)
    BUZZER_PORT &= ~(1 << BUZZER_PIN); // 핀 상태 정리
}

// Timer2 비교 일치 인터럽트 서비스 루틴
ISR(TIMER2_COMPA_vect) {
    BUZZER_PORT ^= (1 << BUZZER_PIN); // PB4 핀 토글
}

void play_delivery_done(void) {
    buzzer_beep(NOTE_E5, T_MELODY);
    buzzer_beep(NOTE_DS5, T_MELODY);
    buzzer_beep(NOTE_E5, T_MELODY);
    buzzer_beep(NOTE_DS5, T_MELODY);
    buzzer_beep(NOTE_E5, T_MELODY);

    buzzer_beep(NOTE_B4, T_MELODY);
    buzzer_beep(NOTE_D5, T_MELODY);
    buzzer_beep(NOTE_C5, T_MELODY);
    buzzer_beep(NOTE_A4, T_MELODY * 2);
}