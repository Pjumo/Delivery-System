#ifndef IR_RECEIVER_H_
#define IR_RECEIVER_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

void ir_init(void);
uint8_t ir_available(void);
uint32_t ir_get_command(void); // 수신된 전체 프레임(Address + Command) 반환

#endif