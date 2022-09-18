/*
 * Project2.c
 *
 *  Created on: Sep 16, 2022
 *      Author: Nada PC
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void increment();

unsigned char sec1, sec2, min1, min2, hour1, hour2, g_tick, flag;

/*
 * each 1 second interrupt happen.
 * increment is called to display the correspond time.
 */
ISR (TIMER1_COMPA_vect) {
	g_tick++;

	if (g_tick >= 1 &&  g_tick < 60) {
		increment();
		g_tick=0;
	}
	if (flag == 1) {
		TCCR1B = 0;
		flag = 0;
	}
}

/*
 * reseting watch.
 */

ISR (INT0_vect) {
	sec1 = 0, sec2 = 0, min1 = 0, min2 = 0, hour1 = 0, hour2 = 0;
}

/*
 * disable timer clock to pause.
 */

ISR (INT1_vect) {
	if (PIND & (1 << PD3)) {
		flag = 1;
	}
}

/*
 * enable timer clock to resume.
 */

ISR (INT2_vect) {
	g_tick = 0;
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
}

/*
 *  timer 1 initialization
 * TCNT1->initial value 0
 * OCR1A->compare with 1000,has top value
 * pre-scaler 1024
 * TIMSK->enable module interrupt bit
 */

void timer1_init_cmp() {

	TCNT1 = 0;
	OCR1A = 1000;
	TIMSK |= (1 << OCIE1A);
	TCCR1A = (1 << FOC1A);
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);

}

/*
 * interrupt 0 initialization
 * internal pull up activate
 * enable interrupt 0
 * with falling edge
 */

void int0_init() {

	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
	GICR |= (1 << INT0);
	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);

}

/*
 * interrupt 1 initialization
 * external pull up with pd2
 * enable interrupt 1
 * with rising edge
 */

void int1_init() {

	DDRD &= ~(1 << PD3);
	PORTD &= ~(1 << PD3);
	GICR |= (1 << INT1);
	MCUCR |= (1 << ISC11) | (1 << ISC10);

}

/*
 * interrupt 2 initialization
 * internal pull up activate
 * enable interrupt 1
 * with falling edge
 */

void int2_init() {

	DDRB &= ~(1 << PB2);
	PORTB |= (1 << PB2);
	GICR |= (1 << INT2);
	MCUCSR &= ~(1 << ISC2);

}

/*
 * calculation of displayed time
 */

void increment() {
	if (sec1 == 9) {
		sec1 = 0;
		if (sec2 == 5) {
			sec2 = 0;
			if (min1 == 9) {
				min1 = 0;
				if (min2 == 5) {
					min2 = 0;
					if (hour1 == 9) {
						hour1 = 0;
						if (hour2 == 9) {
							sec1 = 0, sec2 = 0, min1 = 0, min2 = 0, hour1 = 0, hour2 =
									0;
						} else {
							hour2++;
						}
					} else {
						hour1++;
					}
				} else {
					min2++;
				}
			} else {
				min1++;
			}
		} else {
			sec2++;
		}
	} else {
		sec1++;
	}
}

/*
 * stop watch always display, using round robin algorithm, enabling each pin of port A each 2 ms, 7 segment Multiplexing mode.
 * it appears as always displaying.
 */
void display() {
	PORTA = 0x01;
	PORTC = sec1;
	_delay_ms(2);

	PORTA = 0x02;
	PORTC = sec2;
	_delay_ms(2);

	PORTA = 0x04;
	PORTC = min1;
	_delay_ms(2);

	PORTA = 0x08;
	PORTC = min2;
	_delay_ms(2);

	PORTA = 0x10;
	PORTC = hour1;
	_delay_ms(2);

	PORTA = 0x20;
	PORTC = hour2;
	_delay_ms(2);

}

int main() {

	/*
	 * configuring portA,C as output ports,displaying 0 at first.
	 */
	DDRA |= 0x7F;
	PORTA &= 0x80;

	DDRC |= 0x0F;
	PORTC &= 0xF0;

	int0_init();
	int1_init();
	int2_init();

	timer1_init_cmp();

	SREG |= (1 << 7);

	while (1) {
		display();
	}

	return 0;
}
