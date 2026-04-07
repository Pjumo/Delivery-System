#ifndef SERVO_H_
#define SERVO_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// 서보모터 연결 핀: PD3 (Digital 3번, OC2B)
#define SERVO_DDR   DDRD
#define SERVO_PIN   PD3

void servo_init(void);
void servo_write(uint8_t angle);

#endif