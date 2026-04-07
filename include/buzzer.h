#ifndef BUZZER_H_
#define BUZZER_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// 음계 정의
#define NOTE_E5  659
#define NOTE_DS5 622
#define NOTE_B4  494
#define NOTE_D5  587
#define NOTE_C5  523
#define NOTE_A4  440

// 박자 정의 (ms)
#define T_MELODY 160
#define T_PAUSE  50

// 부저 연결 핀: PB4 (Digital 12)
#define BUZZER_DDR   DDRB
#define BUZZER_PORT  PORTB
#define BUZZER_PIN   PB4

void buzzer_init(void);
void buzzer_beep(uint16_t frequency, uint16_t duration_ms);
void play_delivery_done(void);

#endif