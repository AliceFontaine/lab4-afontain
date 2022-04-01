/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

#include "LCD_GFX.h"
#include "ST7735.h"
#include <math.h>
#include <stdlib.h>
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

/*--------------------Libraries---------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"

/*--------------------Variables---------------------------*/
char String[25];

/******************************************************************************
* Local Functions
******************************************************************************/



/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
			uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if ((pixels>>j)&(1==1)){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
{
	for (int i = 0; i < 16; i++){
		int x = radius * cos(i * 22.5 * 3.1415 / 180);
		int y = radius * sin(i * 22.5 * 3.1415 / 180);
		LCD_drawPixel(x0 + x, y0 + y, color);
	}
}


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
{
	int16_t dx = x1 - x0;
	int16_t dy = y1 - y0;
	int16_t yi = 1;
	int16_t xi = 1;
	
	//double slope = yD / xD;
	
	int16_t steep = abs(dy) > abs(dx);
	//int16_t nx, ny;
	
	if(steep){
		int16_t x = x0;
		if (dx < 0){
			xi = -1;
			dx = -dx;
		}
		
		int16_t D = (2*dx) - dy;
		if (y0<y1){
			for (int y=y0;y<=y1;y++) {
				LCD_drawPixel(x, y, c);
				if (D > 0){
					x = x + xi;
					D = D + (2 * (dx - dy));
				}
				else {
					D = D + 2*dx;
				}
			}
		}
		else{
			dy = -dy;
			for (int y=y0;y>=y1;y--) {
				LCD_drawPixel(x, y, c);
				if (D > 0){
					x = x + xi;
					D = D + (2 * (dx - dy));
				}
				else {
					D = D + 2*dx;
				}
			}
		}
		
	}
	else {
		int16_t y = y0;
		if (dy < 0){
			yi = -1;
			dy = -dy;
		}
		
		int16_t D = (2*dy) - dx;
		
		for (int x=x0;x<=x1;x++) {
			LCD_drawPixel(x, y, c);
			if (D > 0){
				y = y + yi;
				D = D + (2 * (dy - dx));
			}
			else {
				D = D + 2*dy;
			}
		}
	}
}



/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
	
	for (int x = x0; x <= x1; x++){
		for (int y = y0; y <= y1; y++){
			LCD_drawPixel(x, y, color);
		}
	}
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color) 
{
	for (int i=0; i<160; i++){
		for (int j=0; j<128; j++){
			LCD_drawPixel(i, j, color);
		}
	}
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
	char* t; 
	int nx = x;
	for (t = str; *t != '\0'; t++) {
		LCD_drawChar(nx, y, *t, fg, bg);
		nx = nx + 5;
		}
	
}