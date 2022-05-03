/*	Author: Thomas Sullivan - tsull010@ucr.edu
*  Partner(s) Name:
*	Lab Section: 023
*	Assignment: Final Project
*
*	I acknowledge all content contained herein, excluding template or example
*	code, is my own original work.
*
*  Checkpoint 1 Video: https://www.youtube.com/watch?v=MI3y2BJzFzE
*  Checkpoint 2 Video: https://www.youtube.com/watch?v=Pqv4gIvSn-8
*  Checkpoint 3 Video: https://www.youtube.com/watch?v=zd-Ld9mCfoo
*
*  Full Demo: https://www.youtube.com/watch?v=3I2peg5MazA
*/
#include <avr/io.h>
#include "simAVRHeader.h"
#include "timer.h"
#include "io.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#ifdef _SIMULATE_
#endif

////////////////////////////////////////////////////////////

//Global Variables and Declarations

////////////////////////////////////////////////////////////

typedef struct task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
} task;

//this is the chart for the game, for the sake of the demo i've commented out the last 30% of it
const unsigned char chart[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x10, 0x00, 0x01, 0x00,
										 0x44, 0x00, 0x10, 0x00, 0x01, 0x00, 0x04, 0x00,
										 0x50, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x20, 0x00, 0x02, 0x00,
										 0x48, 0x00, 0x10, 0x00, 0x01, 0x00, 0x04, 0x00,
									 	 0x50, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x20, 0x00, 0x04, 0x00,
										 0x48, 0x00, 0x20, 0x00, 0x04, 0x00, 0x02, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x20, 0x00, 0x01, 0x00,
										 0x44, 0x00, 0x20, 0x00, 0x01, 0x00, 0x10, 0x00,
										 0x42, 0x00, 0x01, 0x00, 0x08, 0x00, 0x04, 0x00,
										 0x42, 0x00, 0x10, 0x00, 0x04, 0x00, 0x02, 0x00,
										 0x41, 0x00, 0x20, 0x00, 0x08, 0x00, 0x04, 0x00,
										 0x50, 0x00, 0x02, 0x00, 0x20, 0x00, 0x08, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x10, 0x00, 0x02, 0x00,
										 0x60, 0x00, 0x10, 0x00, 0x08, 0x00, 0x04, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x21, 0x00, 0x04, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x03, 0x00, 0x08, 0x00,
										 0x44, 0x00, 0x02, 0x00, 0x11, 0x00, 0x04, 0x00, //01:16:955
										 0x42, 0x00, 0x10, 0x00, 0x41, 0x00, 0x02, 0x00,
										 0x44, 0x00, 0x20, 0x00, 0x03, 0x00, 0x08, 0x00,
										 0x60, 0x00, 0x02, 0x00, 0x05, 0x00, 0x10, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x11, 0x00, 0x04, 0x00, //01:18:503
										 0x60, 0x00, 0x10, 0x00, 0x09, 0x00, 0x04, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x21, 0x00, 0x02, 0x00,
										 0x48, 0x00, 0x10, 0x00, 0x05, 0x00, 0x20, 0x00,
										 0x44, 0x00, 0x02, 0x00, 0x11, 0x00, 0x04, 0x00,
										 0x48, 0x00, 0x02, 0x00, 0x05, 0x00, 0x02, 0x00,
										 0x50, 0x00, 0x04, 0x00, 0x21, 0x00, 0x08, 0x00,
										 0x42, 0x00, 0x10, 0x00, 0x05, 0x00, 0x02, 0x00,
										 0x60, 0x00, 0x08, 0x00, 0x05, 0x00, 0x02, 0x00,
										 0x50, 0x00, 0x08, 0x00, 0x45, 0x00, 0x02, 0x00,
										 0x11, 0x00, 0x44, 0x00, 0x02, 0x00, 0x18, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x22, 0x00, 0x48, 0x00,
										 0x11, 0x00, 0x0C, 0x00, 0x22, 0x00, 0x08, 0x00,
										 0x11, 0x00, 0x48, 0x00, 0x01, 0x00, 0x0A, 0x00,
										 0x41, 0x00, 0x24, 0x00, 0x11, 0x00, 0x48, 0x00,
										 0x01, 0x00, 0x06, 0x00, 0x41, 0x00, 0x02, 0x00,
										 0x11, 0x00, 0x48, 0x00, 0x22, 0x00, 0x14, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x0A, 0x00, 0x04, 0x00,
										 0x11, 0x00, 0x48, 0x00, 0x02, 0x00, 0x14, 0x00,
										 0x41, 0x00, 0x08, 0x00, 0x22, 0x00, 0x14, 0x00,
										 0x41, 0x00, 0x14, 0x00, 0x22, 0x00, 0x08, 0x00,
										 0x41, 0x00, 0x24, 0x00, 0x02, 0x00, 0x08, 0x00,
										 0x41, 0x00, 0x24, 0x00, 0x02, 0x00, 0x48, 0x00,
										 0x21, 0x00, 0x14, 0x00, 0x0A, 0x00, 0x44, 0x00,
										 0x01, 0x00, 0x14, 0x00, 0x42, 0x00, 0x08, 0x00,
										 0x22, 0x00, 0x48, 0x00, 0x11, 0x00, 0x0C, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x21, 0x00, 0x04, 0x00,
										 0x12, 0x00, 0x48, 0x00, 0x02, 0x00, 0x18, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x22, 0x00, 0x48, 0x00,
										 0x12, 0x00, 0x48, 0x00, 0x02, 0x00, 0x18, 0x00, //01:28:568
										 0x41, 0x00, 0x04, 0x00, 0x22, 0x00, 0x48, 0x00,
										 0x11, 0x00, 0x0C, 0x00, 0x22, 0x00, 0x08, 0x00,
										 0x11, 0x00, 0x44, 0x00, 0x01, 0x00, 0x0C, 0x00,
										 0x42, 0x00, 0x28, 0x00, 0x11, 0x00, 0x44, 0x00,
										 0x01, 0x00, 0x0C, 0x00, 0x42, 0x00, 0x08, 0x00,
										 0x12, 0x00, 0x48, 0x00, 0x22, 0x00, 0x18, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x09, 0x00, 0x04, 0x00,
										 0x12, 0x00, 0x48, 0x00, 0x01, 0x00, 0x14, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x21, 0x00, 0x14, 0x00, //01:32:051
										 0x42, 0x00, 0x18, 0x00, 0x21, 0x00, 0x04, 0x00,
										 0x42, 0x00, 0x28, 0x00, 0x01, 0x00, 0x44, 0x00,
										 0x21, 0x00, 0x14, 0x00, 0x09, 0x00, 0x44, 0x00,
										 0x02, 0x00, 0x18, 0x00, 0x41, 0x00, 0x04, 0x00,
										 0x22, 0x00, 0x48, 0x00, 0x11, 0x00, 0x0C, 0x00,
										 0x22, 0x00, 0x04, 0x00, 0x51, 0x00, 0x08, 0x00, //01:34:761
									 /* 0x41, 0x00, 0x04, 0x00, 0x10, 0x00, 0x01, 0x00,
										 0x44, 0x00, 0x10, 0x00, 0x01, 0x00, 0x04, 0x00,
										 0x50, 0x00, 0x02, 0x00, 0x08, 0x00, 0x20, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x20, 0x00, 0x02, 0x00,
										 0x48, 0x00, 0x10, 0x00, 0x01, 0x00, 0x04, 0x00,
										 0x50, 0x00, 0x01, 0x00, 0x04, 0x00, 0x10, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x20, 0x00, 0x04, 0x00,
										 0x48, 0x00, 0x20, 0x00, 0x04, 0x00, 0x02, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x20, 0x00, 0x01, 0x00, //01:37:858
										 0x44, 0x00, 0x20, 0x00, 0x01, 0x00, 0x10, 0x00,
										 0x42, 0x00, 0x01, 0x00, 0x08, 0x00, 0x04, 0x00,
										 0x42, 0x00, 0x10, 0x00, 0x04, 0x00, 0x02, 0x00,
										 0x41, 0x00, 0x02, 0x00, 0x08, 0x00, 0x04, 0x00,
										 0x50, 0x00, 0x02, 0x00, 0x20, 0x00, 0x08, 0x00,
										 0x41, 0x00, 0x04, 0x00, 0x10, 0x00, 0x02, 0x00,
										 0x50, 0x00, 0x08, 0x00, 0x44, 0x00, 0x02, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x21, 0x00, 0x04, 0x00,
										 0x50, 0x00, 0x04, 0x00, 0x03, 0x00, 0x08, 0x00,
										 0x44, 0x00, 0x02, 0x00, 0x11, 0x00, 0x04, 0x00,
										 0x42, 0x00, 0x10, 0x00, 0x05, 0x00, 0x02, 0x00, //01:42:116
										 0x44, 0x00, 0x20, 0x00, 0x03, 0x00, 0x08, 0x00,
										 0x60, 0x00, 0x02, 0x00, 0x05, 0x00, 0x10, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x11, 0x00, 0x04, 0x00,
										 0x60, 0x00, 0x10, 0x00, 0x09, 0x00, 0x04, 0x00,
										 0x42, 0x00, 0x08, 0x00, 0x21, 0x00, 0x02, 0x00,
										 0x48, 0x00, 0x20, 0x00, 0x05, 0x00, 0x10, 0x00,
										 0x44, 0x00, 0x02, 0x00, 0x09, 0x00, 0x04, 0x00,
										 0x42, 0x00, 0x20, 0x00, 0x05, 0x00, 0x02, 0x00,
										 0x50, 0x00, 0x02, 0x00, 0x21, 0x00, 0x08, 0x00,
										 0x42, 0x00, 0x10, 0x00, 0x05, 0x00, 0x02, 0x00,
										 0x60, 0x00, 0x04, 0x00, 0x11, 0x00, 0x02, 0x00,
										 0x60, 0x00, 0x10, 0x00, 0x49, 0x00, 0x04, 0x00,*/
										 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
										 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
									 	 };
unsigned char gameFlag = 0x00;
unsigned char countFlag = 0x00;
unsigned short combo = 0x00;
unsigned char comboDispA = 0x00;
unsigned char comboDispB = 0x00;
unsigned char comboDispC = 0x00;
unsigned char outputArray[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned short personalBest = 0x00;
////////////////////////////////////////////////////////////

//EEPROM Handling

////////////////////////////////////////////////////////////

unsigned char EEPROM_Read(unsigned char readAddress) {
	EEAR = 0x40 | readAddress;
	EECR |= (1 << EERE);
	return EEDR;
}

void EEPROM_Write(unsigned char writeAddress, unsigned char writeData) {
	EEAR = 0x40 | writeAddress;
	EEDR = writeData;
	EECR = 0x04;
	EECR = 0x02;
	_delay_ms(4);
}

////////////////////////////////////////////////////////////

//LCD Custom Character and Menu Display

////////////////////////////////////////////////////////////

void WriteCustom(unsigned char customChar[], unsigned char offset) {
	LCD_WriteCommand(0x40 | (offset << 3)); // Write to CGROM with address offset
	for (unsigned char i = 0; i < 8; ++i) {
		LCD_WriteData(customChar[i]);
	}
}

void LCD_PrintTitle(unsigned column) {
	unsigned char customCharA[8] = { // b
		0x18,
		0x18,
		0x18,
		0x1E,
		0x1F,
		0x1B,
		0x1B,
		0x16
	};
	unsigned char customCharB[8] = { // r
		0x00,
		0x00,
		0x00,
		0x0E,
		0x1F,
		0x1B,
		0x18,
		0x18
	};
	unsigned char customCharC[8] = { // e
		0x00,
		0x00,
		0x00,
		0x0E,
		0x1B,
		0x1F,
		0x18,
		0x0E
	};
	unsigned char customCharD[8] = { // a
		0x00,
		0x00,
		0x00,
		0x0E,
		0x1F,
		0x1B,
		0x1B,
		0x0D
	};
	unsigned char customCharE[8] = { // d
		0x03,
		0x03,
		0x03,
		0x0F,
		0x1F,
		0x1B,
		0x1B,
		0x0D
	};
	unsigned char customCharF[8] = { // m
		0x00,
		0x00,
		0x00,
		0x11,
		0x1B,
		0x1F,
		0x1B,
		0x1B
	};
	unsigned char customCharG[8] = { // n
		0x00,
		0x00,
		0x00,
		0x1C,
		0x1E,
		0x1B,
		0x1B,
		0x1B
	};
	unsigned char customCharH[8] = { // i
		0x00,
		0x00,
		0x0C,
		0x00,
		0x1C,
		0x0C,
		0x0C,
		0x1F
	};

	unsigned char *customArr[8];
	customArr[0] = customCharA;
	customArr[1] = customCharB;
	customArr[2] = customCharC;
	customArr[3] = customCharD;
	customArr[4] = customCharE;
	customArr[5] = customCharF;
	customArr[6] = customCharG;
	customArr[7] = customCharH;

	for (unsigned char i = 0; i < 8; ++i) {
		WriteCustom(customArr[i], i);
	}

	LCD_Cursor(column);
	LCD_WriteData(0);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(1);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(2);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(3);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(4);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(5);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(3);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(6);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(7);
	++column;
	LCD_Cursor(column);
	LCD_WriteData(3);
	++column;
}

void DispMenu() {
	const unsigned char menuString[] = " Play    Options";
	LCD_DisplayString(17, menuString);
	LCD_PrintTitle(4);
	LCD_Cursor(0);
}

void DispOptions() {
	const unsigned char optString[] = "     Reset Score            Back";
	LCD_DisplayString(1, optString);
	LCD_Cursor(0);
}

void DispGameStandby() {
	const unsigned char sbString[] = "Record:          Play       Back";
	LCD_DisplayString(1, sbString);
	LCD_Cursor(9);
	LCD_WriteData(EEPROM_Read(0x02) | 0x30);
	LCD_Cursor(10);
	LCD_WriteData(EEPROM_Read(0x01) | 0x30);
	LCD_Cursor(11);
	LCD_WriteData(EEPROM_Read(0x00) | 0x30);
	LCD_Cursor(0);
}

void DispGameCombo() {
	const unsigned char comboString[] = "Notes Hit:      Hit 130 to win!";
	LCD_DisplayString(1, comboString);
	LCD_Cursor(0);
}

void DispGameOver() {
	const unsigned char gameWin[] = "    You Win!          notes hit";
	const unsigned char gameLose[] = "   You Lose...        notes hit";
	if (combo >= 130) {
		LCD_DisplayString(1, gameWin);
	}
	else {
		LCD_DisplayString(1, gameLose);
	}
	LCD_Cursor(19);
	LCD_WriteData(comboDispC + '0');
	LCD_Cursor(20);
	LCD_WriteData(comboDispB + '0');
	LCD_Cursor(21);
	LCD_WriteData(comboDispA + '0');
	LCD_Cursor(0);
}

////////////////////////////////////////////////////////////

//ADC Initialization and ADC Mux Selection

////////////////////////////////////////////////////////////

void ADC_init() { // sourced code from Lab 8
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

void ADC_SwitchMux(unsigned char channel) {
	ADMUX = channel & 0x1F;
	ADCSRA |= (1 << ADSC);
	_delay_us(15);
}

////////////////////////////////////////////////////////////

//Joystick Functions

////////////////////////////////////////////////////////////

unsigned char GetJoystick() {
	static unsigned char tmpA;
	static unsigned char tmpB;

	ADC_SwitchMux(0x00);
	tmpA = ADC;
	ADC_SwitchMux(0x01);
	tmpB = ADC;

	if ((tmpB & 0xF0) == 0xE0) {
		return 0x08; //left
	}
	else if ((tmpB & 0x1F) == 0x1F) {
		return 0x01; //right
	}

	if ((tmpA & 0xF0) == 0xE0) {
		return 0x04; //down
	}
	else if ((tmpA & 0x1F) == 0x1F) {
		return 0x02; //up
	}

	if ((tmpA == 0x38) && (tmpB == 0x38)) {
		return 0x00; //no input
	}
	else {
		return 0xFF; //error
	}
}

////////////////////////////////////////////////////////////

//Shift Register Functions

////////////////////////////////////////////////////////////

void FlushShift() {
	for (unsigned char i = 0; i < 16; ++i) {
		PORTD = 0x00;
		PORTD = 0x04;
		PORTD = 0x02;
	}
	PORTD = 0x00;
}

void ShiftChar(unsigned char shiftMe) {
	unsigned char value = 0x00;
	for (unsigned char i = 0; i < 8; ++i) {
		value = (shiftMe >> (i)) & 0x01;
		PORTD = value;
		PORTD = value | 0x04;
		PORTD = 0x02;
		PORTD = 0x00;
	}
}

void DrawLED(unsigned char cols[]) {
	for (unsigned i = 0; i < 8; ++i) {
		ShiftChar(~(0x80 >> i));
		ShiftChar(cols[i]);
		_delay_us(500);
	}
}

////////////////////////////////////////////////////////////

//Project synchSM

////////////////////////////////////////////////////////////

enum Menu_States {Menu_Init, Menu_Base, Menu_OptWait, Menu_Opt, Menu_BaseWait,
	 					Menu_GameSWait, Menu_GameStandby, Menu_GameOWait, Menu_GameOn, Menu_GameOver} Menu_State;

int MenuSMTick(int state) {
	static unsigned char cursorLoc = 0x00;

	switch (state) {
		case Menu_Init:
			personalBest = EEPROM_Read(0x03);
			DispMenu();
			LCD_Cursor(17);
			LCD_WriteData(0x3E);
			LCD_Cursor(0);
			cursorLoc = 17;
			gameFlag = 0x00;
			state = Menu_Base;
		break;
		case Menu_Base:
			gameFlag = 0x00;
			if (((~PINA & 0x04) == 0x04) && (cursorLoc == 25)) {
				LCD_ClearScreen();
				LCD_Cursor(0);
				state = Menu_OptWait;
				break;
			}
			if (((~PINA & 0x04) == 0x04) && (cursorLoc == 17)) {
				LCD_ClearScreen();
				LCD_Cursor(0);
				state = Menu_GameSWait;
				break;
			}

			if (GetJoystick() == 0x01) {
				LCD_Cursor(17);
				LCD_WriteData(0x20);
				LCD_Cursor(25);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 25;
			}
			else if (GetJoystick() == 0x08) {
				LCD_Cursor(25);
				LCD_WriteData(0x20);
				LCD_Cursor(17);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 17;
			}
		break;
		case Menu_OptWait:
			if ((~PINA & 0x04) == 0x00) {
				DispOptions();
				LCD_Cursor(28);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 28;
				state = Menu_Opt;
			}
		break;
		case Menu_Opt:
			if (((~PINA & 0x04) == 0x04) && (cursorLoc == 28)) {
				LCD_ClearScreen();
				LCD_Cursor(0);
				state = Menu_BaseWait;
				break;
			}
			if (((~PINA & 0x04) == 0x04) && (cursorLoc == 5)) {
				while((~PINA & 0x04) == 0x04);
				EEPROM_Write(0x00, 0x00);
				EEPROM_Write(0x01, 0x00);
				EEPROM_Write(0x02, 0x00);
				EEPROM_Write(0x03, 0x00);
				EEPROM_Write(0x04, 0x00);
				LCD_Cursor(0);
			}

			if ((GetJoystick() == 0x02)) {
				LCD_Cursor(28);
				LCD_WriteData(0x20);
				LCD_Cursor(5);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 5;
			}
			else if ((GetJoystick() == 0x04)) {
				LCD_Cursor(5);
				LCD_WriteData(0x20);
				LCD_Cursor(28);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 28;
			}
		break;
		case Menu_BaseWait:
			if ((~PINA & 0x04) == 0x00) {
				gameFlag = 0x00;
				combo = 0;
				comboDispA = 0;
				comboDispB = 0;
				comboDispC = 0;
				DispMenu();
				LCD_Cursor(17);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 17;
				state = Menu_Base;
			}
		break;
		case Menu_GameSWait:
			if ((~PINA & 0x04) == 0x00) {
				DispGameStandby();
				LCD_Cursor(17);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 17;
				state = Menu_GameStandby;
			}
		break;
		case Menu_GameStandby:
			if (((~PINA & 0x04) == 0x04) && (cursorLoc == 28)) {
				LCD_ClearScreen();
				LCD_Cursor(0);
				state = Menu_BaseWait;
				break;
			}
			if (((~PINA & 0x04) == 0x04) && (cursorLoc == 17)) {
				LCD_ClearScreen();
				LCD_Cursor(0);
				combo = 0;
				state = Menu_GameOWait;
				break;
			}

			if (GetJoystick() == 0x01) {
				LCD_Cursor(17);
				LCD_WriteData(0x20);
				LCD_Cursor(28);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 28;
			}
			else if (GetJoystick() == 0x08) {
				LCD_Cursor(28);
				LCD_WriteData(0x20);
				LCD_Cursor(17);
				LCD_WriteData(0x3E);
				LCD_Cursor(0);
				cursorLoc = 17;
			}
		break;
		case Menu_GameOWait:
			if ((~PINA & 0x04) == 0x00) {
				DispGameCombo();
				for (unsigned char i = 0; i < 8; ++i) {
					outputArray[i] = 0x00;
				}
				state = Menu_GameOn;
				gameFlag = 0x01;
			}
		break;
		case Menu_GameOn:
			if ((~PINA & 0x04) == 0x04) {
				gameFlag = 0x00;
				state = Menu_BaseWait;
				break;
			}
			if (!gameFlag) {
				LCD_ClearScreen();
				if (combo > (0x0000 | EEPROM_Read(0x03) | EEPROM_Read(0x04))) {
					EEPROM_Write(0x00, comboDispA);
					EEPROM_Write(0x01, comboDispB);
					EEPROM_Write(0x02, comboDispC);
					EEPROM_Write(0x03, combo >> 8);
					EEPROM_Write(0x04, combo & 0x00FF);
				}
				DispGameOver();
				state = Menu_GameOver;
			}
		break;
		case Menu_GameOver:
			if ((~PINA & 0x04) == 0x04) {
				state = Menu_BaseWait;
			}
		break;
		default:
			state = Menu_Init;
		break;
	}
	return state;
}

enum LED_States {LED_Base, LED_Draw} LED_State;

int LEDSMTick(int state) {
	static unsigned char ledCnt = 0x00;

	switch (state) {
		case LED_Base:
			FlushShift();
			if (gameFlag) {
				state = LED_Draw;
				break;
			}
		break;
		case LED_Draw:
			if (!gameFlag) {
				state = LED_Base;
				break;
			}
			FlushShift();
			ShiftChar(~(0x80 >> ledCnt));
			ShiftChar(outputArray[ledCnt]);
			++ledCnt;
			if (ledCnt >= 8) {
				ledCnt = 0;
			}
		break;
		default:
			state = LED_Base;
		break;
	}
	return state;
}

enum Game_States {Game_Off, Game_On} Game_State;

int GameSMTick(int state) {
	static unsigned long gameCntA = 0x00;
	switch (state) {
		case Game_Off:
			gameCntA = 0x00;
			if (gameFlag) {
				for (unsigned char x = 0x00; x < 8; ++x) {
					outputArray[x] = 0x00;
				}
				state = Game_On;
			}
		break;
		case Game_On:
			if (!gameFlag) {
				FlushShift();
				state = Game_Off;
				break;
			}
			++gameCntA;
			for (unsigned char gameCntB = 0; gameCntB < 8; ++gameCntB) {
				if (gameCntB == 7) {
					outputArray[0] = chart[gameCntA - 1];
					break;
				}
				outputArray[7 - gameCntB] = outputArray[6 - gameCntB];
			}
			countFlag = 0;
			LCD_Cursor(12);
			LCD_WriteData(comboDispC + '0');
			LCD_Cursor(13);
			LCD_WriteData(comboDispB + '0');
			LCD_Cursor(14);
			LCD_WriteData(comboDispA + '0');
			if (combo > 130) {
				LCD_Cursor(15);
				LCD_WriteData(0x21);
			}
			LCD_Cursor(0);
			if (gameCntA >= 570) {
				gameFlag = 0x00;
			}
		break;
		default:
			state = Game_Off;
		break;
	}
	return state;
}

enum Input_States {Input_Off, Input_On} Input_State;

int InputSMTick(int state) {
	static unsigned char userInput = 0x00;
	switch (state) {
		case Input_Off:
			userInput = 0x00;
			if (gameFlag) {
				state = Input_On;
			}
		break;
		case Input_On:
			if (!gameFlag) {
				state = Input_Off;
				break;
			}

			userInput = (~PINB & 0x80);
			userInput >>= 1;
			userInput |= (~PINB & 0x3F);

			if (userInput == 0x7F) {
			}
			else if (!countFlag){
				if (userInput != 0x00) {
					for (unsigned char i = 0; i < 8; ++i) {
						if (userInput == outputArray[7 - i]) {
							++combo;
							if (comboDispA < 9) {
								++comboDispA;
							}
							else {
								comboDispA = 0;
								if (comboDispB < 9) {
									++comboDispB;
								}
								else {
									comboDispB = 0;
									++comboDispC;
								}
							}
							countFlag = 1;
							break;
						}
					}
				}
			}
		break;
		default:
			state = Input_Off;
		break;
	}
	return state;
}


////////////////////////////////////////////////////////////

//findGCD and main Function

////////////////////////////////////////////////////////////

unsigned long int findGCD(unsigned long int a, unsigned long int b) {
	unsigned long int c;
	while (1) {
		c = a % b;
		if(c == 0) {return b;}
		a = b;
		b = c;
	}
	return 0;
}

int main(void) {
	DDRA = 0x18; PORTA = 0xE7;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	LCD_init();
	ADC_init();

	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	task1.state = Menu_Init;
	task1.period = 30;
	task1.elapsedTime = task1.period;
	task1.TickFct = &MenuSMTick;

	task2.state = LED_Base;
	task2.period = 2;
	task2.elapsedTime = task2.period;
	task2.TickFct = &LEDSMTick;

	task3.state = Game_Off;
	task3.period = 50;
	task3.elapsedTime = task3.period;
	task3.TickFct = &GameSMTick;

	task4.state = Input_Off;
	task4.period = 10;
	task4.elapsedTime = task4.period;
	task4.TickFct = &InputSMTick;

	unsigned short i;
	unsigned long GCD = tasks[0]->period;
	for (i = 1; i < numTasks; i++) {
		GCD = findGCD(GCD,tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();

	while (1) {
		for (i = 0; i < numTasks; i++) {
			if (tasks[i]->elapsedTime == tasks[i]->period) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += GCD;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}
