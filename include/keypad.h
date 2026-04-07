#ifndef KEYPAD_H_
#define KEYPAD_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void keypad_init(void);
char keypad_scan(void);

#endif