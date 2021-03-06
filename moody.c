#include <avr/io.h>
#include <stdint.h> 
#include <avr/interrupt.h> 
#include <avr/pgmspace.h>

#define BUTTON1 (1<<PC5) 
#define BUTTON2 (1<<PC4) 
#define BUTTON3 (1<<PC3) 
#define BUTTON4 (1<<PC2) 

#define GREEN	(1<<PB1) 
#define BLUE	(1<<PB2) 
#define RED	(1<<PB3) 

volatile size_t fade = 0;
volatile uint32_t multi;
volatile uint16_t op = 1;
volatile uint16_t val;

volatile uint8_t colors = 0;
volatile uint8_t cRed = 0;
volatile uint8_t cGreen = 0;
volatile uint8_t cBlue = 0;

volatile uint8_t buttons = 0;
volatile uint8_t cButton1 = 0;
volatile uint8_t cButton2 = 0;
volatile uint8_t cButton3 = 0;
volatile uint8_t cButton4 = 0;

void init() {

	// enable LED
	DDRB |= GREEN;
	DDRB |= BLUE;
	DDRB |= RED;

	// button pullups
	PORTC |= BUTTON1;
	PORTC |= BUTTON2;
	PORTC |= BUTTON3;
	PORTC |= BUTTON4;

	TIMSK0 |= (1<<TOIE0);
	TCCR0B |= (1<<CS00); 

	TIMSK1 |= (1<<TOIE1);
//	TCCR1B |= (1<<CS00); 
	TCCR1B |= (1<<CS01); 

	TIMSK2 |= (1<<TOIE2);
	TCCR2B |= (1<<CS00); 
	TCCR2B |= (1<<CS01); 


	sei();

}

const uint16_t pwmtable[64] = {
    0,  1,  1,  1,  1,  1,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  4,  4,  5,  5,  5,
    6,  7,  7,  8,  9,  9, 10, 11, 12, 13,
   14, 16, 17, 19, 21, 22, 24, 27, 29, 32,
   35, 38, 41, 45, 49, 54, 58, 64, 70, 76,
   83, 90, 99,107,117,128,139,152,166,181,
  197,215,235,256
};

ISR(TIMER0_OVF_vect) {

	if (colors & RED) {
		if(cRed < pwmtable[fade]) {
			cRed = cRed + 1;
			PORTB &= ~RED;
		} else {
			cRed = 0;
			PORTB |= RED;
		}
	}

	if (colors & GREEN) {
		if(cGreen < pwmtable[fade]) {
			cGreen = cGreen + 1;
			PORTB &= ~GREEN;
		} else {
			cGreen = 0;
			PORTB |= GREEN;
		}
	}

	if (colors & BLUE) {
		if(cBlue < pwmtable[fade]) {
			cBlue = cBlue + 1;
			PORTB &= ~BLUE;
		} else {
			cBlue = 0;
			PORTB |= BLUE;
		}
	}

}

ISR(TIMER1_OVF_vect) {
	multi= multi +1;
	
	if(multi>=1) {
		val=1*op;
		multi=0;
		fade = fade + val;

			if(fade <= 1) {
				fade=1;
				op=1;
			}
			if(fade >= 63) {
				fade=63;
				op=-1;
			}
	}
}

ISR(TIMER2_OVF_vect) {

	if (!(PINC & BUTTON1)) {
		cButton1 = cButton1 + 1;
	} else if ((PINC & BUTTON1) && cButton1 > 10) {
		buttons |= BUTTON1;
		cButton1 = 0;
	} else if (!(PINC & BUTTON2)) {
		cButton2 = cButton2 + 1;
	} else if ((PINC & BUTTON2) && cButton2 > 10) {
		buttons |= BUTTON2;
		cButton2 = 0;
	} else if (!(PINC & BUTTON3)) {
		cButton3 = cButton3 + 1;
	} else if ((PINC & BUTTON3) && cButton3 > 10) {
		buttons |= BUTTON3;
		cButton3 = 0;
	} else if (!(PINC & BUTTON4)) {
		cButton4 = cButton4 + 1;
	} else if ((PINC & BUTTON4) && cButton4 > 10) {
		buttons |= BUTTON4;
		cButton4 = 0;
	}

}

int main() { 

	init();

	colors |= GREEN;

	while(1) { 
	
		if (buttons & BUTTON1) {
			buttons ^= BUTTON1;
		}

		if (buttons & BUTTON2) {
			colors ^= RED;
			buttons ^= BUTTON2;
		}

		if (buttons & BUTTON3) {
			colors ^= BLUE;
			buttons ^= BUTTON3;
		}

		if (buttons & BUTTON4) {
			colors ^= GREEN;
			buttons ^= BUTTON4;
		}

	}

}
