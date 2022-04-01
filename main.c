/*
 * ESE350_Lab4_Pong_Starter.c
 *
 * Created: 4/1/2022 1:29 PM
 * Author : Alice Fontaine
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

/*--------------------Libraries---------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart.h"

/*--------------------Variables---------------------------*/
	int up = 1;
	int r = 0;
	int bx = 80;
	int by = 70;
	int br = 2;
	int px = 5;
	int y = 50;
	int cy = 50;
	int cx = 152;
	int cpoints = 0;
	int ppoints = 0;
	int Round = 1;
	int gr = 0;
	int re = 0;
	
	char pString[25];
	char cString[25];
	char prString[25];
	char crString[25];

void Initialize()
{
	UART_init(BAUD_PRESCALER); 
	

	cli();
	
	//OUTPUT Green LED
	DDRB &= ~(1<<DDD4);
	//OUTPUT Red LED
	DDRB &= ~(1<<DDD3);
	
	//UP
	DDRC &= ~(1<<DDC5);
	//DOWN
	DDRC &= ~(1<<DDC4);
	
	//Output Buzzer Pin
	DDRD &= ~(1<<DDD5);
	OCR0A = 14;
	OCR0B = OCR0A * 0.25;
	
	//TIMER 0 prescaled
	TCCR0B |= (1<<CS02);
	
	//Timer 0 is FAST PWM
	
	TCCR0A |= (1<<WGM00);
	TCCR0B |= (1<<WGM02);
	
	//TCCR1A |= (1<<COM1A0);
	
	TCCR0A |= (1<<COM0B1);
	//TIMSK0 |= (1<<OCIE0A);
	
	//Input Joystick Pin PA0
	//ADC SET-UP
	PRR &= ~(1<<PRADC);
	
	//Select Vref = AVcc
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	
	//Set clock prescale by 128
	ADCSRA |= (1<<ADPS0);
	ADCSRA |= (1<<ADPS1);
	ADCSRA |= (1<<ADPS2);
	
	//Select channel 0
	ADMUX &= ~(1<<MUX0);
	ADMUX &= ~(1<<MUX1);
	ADMUX &= ~(1<<MUX2);
	ADMUX &= ~(1<<MUX3);
	
	//Set to free running
	ADCSRA |= (1<<ADATE);
	ADCSRB &= ~(1<<ADTS0);
	ADCSRB &= ~(1<<ADTS1);
	ADCSRB &= ~(1<<ADTS2);
	
	//Disable digital input buffer
	DIDR0 |= (1<<ADC0D);
	
	//Enable ADC
	ADCSRA |= (1<<ADEN);
	
	// Start Conversion
	ADCSRA |= (1<<ADSC);
	
	//Enable interrupt
	ADCSRA |= (1<<ADIE);
	
	sei();
}

ISR(ADC_vect)
{
	//ADC Y stick range from 0-1000, up is 0, down is 1000, neutral position is 500.
	if (ADC < 450){
		//Move paddle up
	}
	if (ADC > 550){
		//Move paddle down
	}
}

void beep(){
	PORTD |= (1<<DDD5);
	Delay_ms(50);
	PORTD &= ~(1<<DDD5);
}

int hitsPaddle(){
	if (bx < (px + 5)){
		if ((by > y) && (by < (y + 30))){
			beep();
			return 1;		
		}
	}
	if (bx > (cx - 3)){
		if ((by > cy) && (by < (cy + 30))){
			beep();
			return 1;
		}
	}
	return 0;
}

void playRound(){
	
	up = 1;
	r = 0;
	bx = 80;
	by = 70;
	br = 2;
	px = 5;
	y = 50;
	cy = 50;
	cx = 152;
	
	sprintf(pString, "Green:%u", ppoints);
	sprintf(cString, "Red:%u", cpoints);
	
	sprintf(prString, "%u", gr);
	sprintf(crString, "%u", re);
	
	LCD_setScreen(0XFFFF);
	//POINTS
	LCD_drawString(5, 5, pString, 0X6F6F, 0XFFFF);
	LCD_drawString(130, 5, cString, 0XE8E8, 0XFFFF);
	//ROUND
	LCD_drawString(74, 120, prString, 0X6F6F, 0XFFFF);
	LCD_drawString(80, 120, crString, 0XE8E8, 0XFFFF);
	LCD_drawString(63, 5, "PONG", 500, 0XFFFF);
	LCD_drawLine(0, 13, 159, 13, 0XF9F9);
	LCD_drawBlock(px, cy, px + 2, cy + 30, 0X6F6F);
	LCD_drawBlock(cx, cy, cx + 2, cy + 30, 0XE8E8);
	LCD_drawCircle(bx, by, br, 0X0000);
	
	int ongoing = 1;
	while (ongoing) {
		if (ADC < 450) {
			if (y>14) {
				y--;
				LCD_drawLine(px, y, px + 2, y, 0X6F6F);
				LCD_drawLine(px, y+31, px + 2, y+31, 0XFFFF);
			}
		}
		if (ADC > 550) {
			if (y+30 < 128) {
				y++;
				LCD_drawLine(px, y+30, px + 2, y+30, 0X6F6F);
				LCD_drawLine(px, y-1, px + 2, y-1, 0XFFFF);
			}
		}
		
		//COMPUTER
		/*
		if (up){
			cy--;
			LCD_drawLine(cx, cy, cx + 2, cy, 0XE8E8); //red
			LCD_drawLine(cx, cy+31, cx + 2, cy+31, 0XFFFF);
			if (cy==14){
				up = 0;
			}
		}
		else {
			cy++;
			LCD_drawLine(cx, cy+30, cx + 2, cy + 30, 0XE8E8);
			LCD_drawLine(cx, cy-1, cx + 2, cy-1, 0XFFFF);
			if ((cy + 30) == 126){
				up = 1;
			}
		}
		*/
		// SECOND PLAYER:
		if (PINC & (1<<PINC5)){
			if (cy>14) {
				cy--;
				LCD_drawLine(cx, cy, cx + 2, cy, 0XE8E8); //red
				LCD_drawLine(cx, cy+31, cx + 2, cy+31, 0XFFFF);
			}
		}
		if (PINC & (1<<PINC4)){
			if (cy+30 < 127) {
				cy++;
				LCD_drawLine(cx, cy+30, cx + 2, cy + 30, 0XE8E8);
				LCD_drawLine(cx, cy-1, cx + 2, cy-1, 0XFFFF);
			}
		}
		
		//BALL
		LCD_drawCircle(bx, by, br, 0XFFFF);
		if (r == 0){
			bx--;
			by--;
			if (by==(14+br)){
				beep();
				r = 2;
			}
			if (hitsPaddle()) {
				r = 3;
			}
		}
		else if (r == 1){
			bx++;
			by++;
			if (by==(127-br)){
				beep();
				r = 3;
			}
			if (hitsPaddle()) {
				r = 2;
			}
		}
		else if (r == 2){
			bx--;
			by++;
			if (by==(127-br)){
				beep();
				r = 0;
			}
			if (hitsPaddle()) {
				r = 1;
			}
		}
		else if (r == 3){
			bx++;
			by--;
			if (by==(14+br)){
				beep();
				r = 1;
			}
			if (hitsPaddle()) {
				r = 0;
			}
		}
		LCD_drawCircle(bx, by, br, 0X0000);
		
		if (bx > cx) {
			beep();
			PORTD |= (1<<PORTD4); //Green LED
			Delay_ms(2000);
			PORTD &= ~(1<<PORTD4);
			ppoints++;
			ongoing = 0;
		}
		else if (bx < px){
			beep();
			PORTD |= (1<<PORTD3); //Red LED
			Delay_ms(2000);
			PORTD &= ~(1<<PORTD3);
			cpoints++;
			ongoing = 0;
		}
	}
}

int main(void)
{	
	Initialize();
	lcd_init();
	int playing = 1;
	
	//xffff
	//x0000
	
	/*sprintf(String, "Mode %u  \n", mode);
	UART_putstring(String);*/
	
    while (playing)
    {
		//PLAYER
		playRound();
		if (ppoints == 3){
			LCD_drawString(35, 55, "POINT GREEN", 0X6F6F, 0XFFFF);
			gr++;
			ppoints = 0;
			cpoints = 0;
			Round++;
		}
		else if (cpoints == 3){
			LCD_drawString(45, 55, "POINT RED", 0XE8E8, 0XFFFF);
			re++;
			ppoints = 0;
			cpoints = 0;
			Round++;
		}
		
		if (Round > 3){
			if (gr > re){
				LCD_setScreen(0X6F6F);
				LCD_drawString(35, 55, "GREEN WINS", 0XFFFF, 0X6F6F);
			}
			else {
				LCD_setScreen(0XE8E8);
				LCD_drawString(35, 55, "RED WINS", 0XFFFF, 0XE8E8);
			}
			playing = 0;
		}
    }
}