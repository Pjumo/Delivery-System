#include "servo.h"

void servo_init(void) {
    SERVO_DDR |= (1 << SERVO_PIN); // PD3 출력 설정
}

void servo_write(uint8_t angle) {
    if (angle > 180) angle = 180;

    // --- Timer2 설정 (Servo 모드: Fast PWM) ---
    // 모드 7: Fast PWM (TOP = OCR2A)
    // Prescaler 1024 설정
    TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << WGM22) | (1 << CS22) | (1 << CS21) | (1 << CS20);

    // 주파수 설정: 약 61Hz (20ms에 가깝게 설정)
    // 16MHz / (1024 * (1 + 255)) = 약 61Hz
    OCR2A = 255; 

    // 펄스 폭 계산 (1ms ~ 2ms 범위)
    // 0도: 약 16 (0.5ms), 180도: 약 32 (2ms)
    OCR2B = 16 + (uint16_t)(angle * 16 / 180);

    // 모터가 회전할 시간을 줌 (Blocking을 최소화하려면 호출 측에서 관리)
    _delay_ms(500);

    // 사용 후 타이머 정지 (부저와의 충돌 방지 및 모터 떨림 방지)
    TCCR2B = 0;
    TCCR2A = 0;
    PORTD &= ~(1 << SERVO_PIN);
}