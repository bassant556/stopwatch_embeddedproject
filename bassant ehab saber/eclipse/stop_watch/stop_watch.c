/*
 * stop_watch.c
 *
 *  Created on: Sep 12, 2023
 *      Author: bassant
 */

#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
unsigned char second_units=0;
unsigned char second_tenth=0;
unsigned char minute_units=0;
unsigned char minute_tenth=0;
unsigned char hour_units=0;
unsigned char hour_tenth=0;



void enable_second_units(){
	PORTA&=0xE0;
	PORTA|=(1<<PA5);
	PORTC=(PORTC&0xF0)|(second_units&0x0F);
}

void enable_second_tenth(){
	PORTA&=0xD0;
	PORTA|=(1<<PA4);
	PORTC=(PORTC&0xF0)|(second_tenth&0x0F);
}
void enable_minutes_units(){
	PORTA&=0xC8;
	PORTA|=(1<<PA3);
	PORTC=(PORTC&0xF0)|(minute_units&0x0F);
}
void enable_minutes_tenth(){
	PORTA&=0xC4;
	PORTA|=(1<<PA2);
	PORTC=(PORTC&0xF0)|(minute_tenth&0x0F);

}
void enable_hours_units(){
	PORTA&=0xC2;
	PORTA|=(1<<PA1);
	PORTC=(PORTC&0xF0)|(hour_units&0x0F);
}

void enable_hours_tenth(){

	PORTA&=0xC1;
	PORTA|=(1<<PA0);
	PORTC=(PORTC&0xF0)|(hour_tenth&0x0F);
}
void timer1_seconds_count(void){
	TCCR1A=(1<<FOC1A);//this PIN is enabled when we don't use pwm mode
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS12);//here we enable compare mode and use prescalar 1024
	OCR1A=977;
	//the timer will count 1000 counts as we used prescalar 1024 and the system frequency is 1Mhz so the timer frequency is 1M/1024 ,and the timer counts 1 count every 1ms ,so in one second we have 1000 counts
	TCNT1=0;//intial value for the counter
	TIMSK=(1<<OCIE1A);//interrupt mask register
	SREG|=(1<<7);//I-bit


}
ISR(TIMER1_COMPA_vect){

	second_units++;
	if(second_units==10){
		second_units=0;
		second_tenth++;
	}
	if(second_tenth==6){
		second_tenth=0;
		minute_units++;
	}
	if(minute_units==10){
		minute_units=0;
		minute_tenth++;
	}
	if(minute_tenth==6){
		minute_tenth=0;
		hour_units++;
	}
	if(hour_units==10){
		hour_units=0;
		hour_tenth++;
	}
	if(hour_tenth==10){
		hour_tenth=0;
	}

}


void interrupt0func(void){
	MCUCR|=(1<<ISC01);//falling edge
	GICR|=(1<<INT0);//interrupt control register
	SREG|=(1<<7);//I-bit

}
ISR(INT0_vect){
	//set all the bits by zero to reset timer
	second_units=0;
	second_tenth=0;
	minute_units=0;
	minute_tenth=0;
	hour_units=0;
	hour_tenth=0;
}


void interrupt1func(void){

	MCUCR|=(1<<ISC10)|(1<<ISC11);//rising edge
	GICR|=(1<<INT1);//interrupt control register
	SREG|=(1<<7);//I-bit

}

ISR(INT1_vect){

	TCCR1B=0;//disable all bits as we want to close the clock
}


void interrupt2func(void){
	MCUCR&=~(1<<ISC2);//falling edge
	GICR|=(1<<INT2);//interrupt control register
	SREG|=(1<<7);//I-bit

}
ISR(INT2_vect){
	//reopen the clock
	_delay_ms(30);
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS12);
}
int main(){
	DDRC|=0x0F;// Connecting 7447 decoder 4-pins to the first 4-pins in PORTC
	PORTC&=0xF0;//initialze the four pins to zero
	DDRA|=0x3F;//considering enable pins as input pins
	PORTA|=0x3F;//enable the six 7 segments

	DDRD=0x0C;//consider PD2 ,PD3 as input pins
	DDRB&=~(1<<2);//consider PB2 as input pin
	PORTD|=(1<<2);
	PORTB|=(1<<2);
	timer1_seconds_count();
	interrupt0func();
	interrupt1func();
	interrupt2func();
	while(1){
		enable_second_units();
		_delay_ms(2);

		enable_second_tenth();
		_delay_ms(2);

		enable_minutes_units();
		_delay_ms(2);

		enable_minutes_tenth();
		_delay_ms(2);

		enable_hours_units();
		_delay_ms(2);

		enable_hours_tenth();
		_delay_ms(2);


	}
}




