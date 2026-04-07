#ifndef BUZZER_H_
#define BUZZER_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// 음계 정의
#define NOTE_G5  1568
#define NOTE_GS5 1661
#define NOTE_AS5 1865
#define NOTE_CS6 2217

// 박자 정의 (ms)
#define T_MELODY 230
#define T_PAUSE  50

// 부저 연결 핀: PB4 (Digital 12)
#define BUZZER_DDR   DDRB
#define BUZZER_PORT  PORTB
#define BUZZER_PIN   PB4

void buzzer_init(void);
void buzzer_beep(uint16_t frequency, uint16_t duration_ms);
void play_delivery_done(void);

#endif