#include "buzzer.h"

void buzzer_init(void) {
    BUZZER_DDR |= (1 << BUZZER_PIN); // PB4(12번) 출력 설정
    // 초기에는 타이머를 켜지 않고 대기합니다.
    TCCR2A = 0;
    TCCR2B = 0;
    TIMSK2 &= ~(1 << OCIE2A);
}

void buzzer_beep(uint16_t frequency, uint16_t duration_ms) {
    if (frequency == 0) return;

    uint32_t ocr_calc = (F_CPU / (2 * 64UL * frequency)) - 1;
    if (ocr_calc > 255) {
        ocr_calc = 255; 
    }

    // --- [1] 부저 구동을 위한 Timer2 설정 ---
    TCCR2A = (1 << WGM21);              // CTC 모드
    TCCR2B = (1 << CS22);               // Prescaler 64
    
    // 주파수 계산 및 적용
    OCR2A = (uint8_t)ocr_calc;
    
    TCNT2 = 0;               // 타이머 카운터 초기화
    TIMSK2 |= (1 << OCIE2A); // 인터럽트 활성화 (소리 시작)

    // 지정된 시간만큼 재생
    for (uint16_t i = 0; i < duration_ms; i++) {
        _delay_ms(1);
    }

    // --- [2] 타이머 자원 초기화 및 반납 (핵심!) ---
    TIMSK2 &= ~(1 << OCIE2A);          // 1. 인터럽트 중지
    TCCR2B = 0;                        // 2. 클럭 공급 중단 (타이머 정지)
    TCCR2A = 0;                        // 3. 모드 초기화
    BUZZER_PORT &= ~(1 << BUZZER_PIN); // 4. 핀 상태 LOW로 정리
    
    _delay_ms(20); // 음 간의 명확한 구분을 위한 짧은 휴식
}

// Timer2 비교 일치 인터럽트 서비스 루틴
ISR(TIMER2_COMPA_vect) {
    BUZZER_PORT ^= (1 << BUZZER_PIN); // PB4 핀 토글
}

void play_delivery_done(void) {
     for (int i = 0; i < 3; i++) {
        buzzer_beep(NOTE_G5, T_MELODY); // 솔
        buzzer_beep(NOTE_GS5, T_MELODY); // 라
        buzzer_beep(NOTE_AS5, T_MELODY); // 시♭
    }

    // 마지막 높은 레
    buzzer_beep(NOTE_CS6, T_MELODY + 100);
}