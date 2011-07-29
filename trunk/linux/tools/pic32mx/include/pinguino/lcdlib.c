/*
 * LCD routines for use with pinguino 32X board, based on LiquidCrystal lib from Arduino project.
 * Port by Marcus Fazzi (anunakin@ieee.org)
 * LiquidCrystal original Arduino site: 
 *      http://www.arduino.cc/en/Tutorial/LiquidCrystal by David A. Mellis
 * Pins, Schematics and more info: 
 * 		http://pinguino.koocotte.org/index.php/LCD_Example
 * 		http://www.vivaphp.net
 */

#ifndef __LCDLIB_C__
#define __LCDLIB_C__

//Arduino like delays
#include <delay.c>

//Arduino like DigitalWrite and Read
#include <digitalw.c>

// pinguino stdio own library
#include <stdio.c>
#include <stdarg.h>

#ifndef __LCDLIB_H__
#include <lcdlib.h>
#endif

/** Positive pulse on E */
void lcd_pulseEnable(void) {
  digitalwrite(_enable_pin, LOW);
  Delayus(1);
  digitalwrite(_enable_pin, HIGH);
  Delayus(1);    // enable pulse must be >450ns
  digitalwrite(_enable_pin, LOW);
  Delayus(50);   // commands need > 37us to settle
}

/** Write using 4bits mode */
void lcd_write4bits(unsigned char value) {
	int i;
	for (i = 0; i < 4; i++) {		
		digitalwrite(_data_pins[i], (value >> i) & 0x01);
	}
	lcd_pulseEnable();
}

/** Write using 8bits mode */
void lcd_write8bits(unsigned char value) {
	int i;
	for (i = 0; i < 8; i++) {		
		digitalwrite(_data_pins[i], (value >> i) & 0x01);
	}
	lcd_pulseEnable();
}

/** Send data to LCD 8 or 4 bits */
void lcd_send(unsigned char value, unsigned char mode) {
	digitalwrite(_rs_pin, mode);
    if (_displayfunction & LCD_8BITMODE) {
		lcd_write8bits(value);
	} else {
		lcd_write4bits(value>>4);
		lcd_write4bits(value);
	}
}

/** Write a data character on LCD */
void lcd_write(unsigned char value) {
	lcd_send(value, HIGH);
}

 
/** Write a control command on LCD */
void command(unsigned char value) {
	send(value, LOW);
}

/** Setup line x column on LCD */
void lcd_setCursor(unsigned char col, unsigned char row) {
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

/** Print a string on LCD */
void lcd_print(char *string) {
	int i;
	for( i=0; string[i]; i++) {
		lcd_write(string[i]);
	}
}

/** Write formated string on LCD **/
//  added 23/02/2011 rblanchot@gmail.com
void lcd_printf(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	pprintf(write, fmt, args);
	va_end(args);
}

/** Print a number on LCD */
void lcd_printNumber(unsigned long n, unsigned char base) {  
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
  unsigned long i = 0;

  if (n == 0) {
    lcd_write('0');
    return;
  } 

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    lcd_write((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}

/** Print a float number to LCD */
void lcd_printFloat(float number, unsigned char digits) { 
  unsigned char i, toPrint;
  unsigned long int_part;
  float rounding, remainder;
  
  // Handle negative numbers
  if (number < 0.0) {
     lcd_write('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"  
  rounding = 0.5;
  for (i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it  
  int_part = (unsigned long)number;
  remainder = number - (float)int_part;
  lcd_printNumber(int_part, 10);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    lcd_write('.'); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    toPrint = (unsigned int)remainder; //Integer part without use of math.h lib, I think better! (Fazzi)
    lcd_printNumber(toPrint, 10);
    remainder -= toPrint; 
  }
}

/** Move cursor to Home position */
void lcd_home(){
	lcd_command(LCD_RETURNHOME);
	Delayus(2000);
}

/** Clear LCD */
void lcd_clear() {
  lcd_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  Delayus(2000);  // this command takes a long time!
}

/** Turn the display on/off (quickly) */
void lcd_noDisplay() {
  _displaycontrol &= ~LCD_DISPLAYON;
  lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display() {
  _displaycontrol |= LCD_DISPLAYON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/** Turns the underline cursor on/off */
void lcd_noCursor() {
  _displaycontrol &= ~LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_cursor() {
  _displaycontrol |= LCD_CURSORON;
  lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/** Turn on and off the blinking cursor */
void lcd_noBlink() {
  _displaycontrol &= ~LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_blink() {
  _displaycontrol |= LCD_BLINKON;
  lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/** These commands scroll the display without changing the RAM */
void lcd_scrollDisplayLeft(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcd_scrollDisplayRight(void) {
  lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

/** This is for text that flows Left to Right */
void lcd_leftToRight(void) {
  _displaymode |= LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | _displaymode);
}

/** This is for text that flows Right to Left */
void lcd_rightToLeft(void) {
  _displaymode &= ~LCD_ENTRYLEFT;
  lcd_command(LCD_ENTRYMODESET | _displaymode);
}

/** This will 'right justify' text from the cursor */
void lcd_autoscroll(void) {
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | _displaymode);
}

/** This will 'left justify' text from the cursor */
void lcd_noAutoscroll(void) {
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  lcd_command(LCD_ENTRYMODESET | _displaymode);
}

/** Initial Display settings! */
void lcd_begin(unsigned char lines, unsigned char dotsize) {
  if (lines > 1) {
    _displayfunction |= LCD_2LINE;
  }
  _numlines = lines;
  _currline = 0;

  // Some one line displays can select 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    _displayfunction |= LCD_5x10DOTS;
  }

  Delayus(50000); //Pinguino needs it? long delay on startup time!
  // Now we pull both RS and R/W low to begin commands
  digitalwrite(_rs_pin, LOW);
  digitalwrite(_enable_pin, LOW);
  
  //put the LCD into 4 bit or 8 bit mode
  if (! (_displayfunction & LCD_8BITMODE)) {
    // this is according to the hitachi HD44780 datasheet
    // figure 24, pg 46

    // we start in 8bit mode, try to set 4 bit mode
    lcd_write4bits(0x03);
    Delayus(4500); // wait min 4.1ms

    // second try
    lcd_write4bits(0x03);
    Delayus(4500); // wait min 4.1ms
    
    // third go!
    lcd_write4bits(0x03); 
    Delayus(150);

    // finally, set to 8-bit interface
    lcd_write4bits(0x02); 
  } else {
    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    lcd_command(LCD_FUNCTIONSET | _displayfunction);
    Delayus(4500);  // wait more than 4.1ms

    // second try
    lcd_command(LCD_FUNCTIONSET | _displayfunction);
    Delayus(150);

    // third go
    lcd_command(LCD_FUNCTIONSET | _displayfunction);
  }

  // finally, set # lines, font size, etc.
  lcd_command(LCD_FUNCTIONSET | _displayfunction);  

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
  lcd_display();

  // clear it off
  lcd_clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  lcd_command(LCD_ENTRYMODESET | _displaymode);

}

/** Init LCD 
 * mode 	=> 1 => 4 bits // 0 => 8 bits
 * rs , rw, enable
 * pins => D0 ~ D7.
 */
void lcd_init(unsigned char fourbitmode, unsigned char rs, unsigned char rw, unsigned char enable, 
			unsigned char d0, unsigned char d1, unsigned char d2, unsigned char d3,
			unsigned char d4, unsigned char d5, unsigned char d6, unsigned char d7){
  int i;
  _rs_pin = rs;
  _rw_pin = rw;
  _enable_pin = enable;
  
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7; 

  pinmode(_rs_pin, OUTPUT);
  
  pinmode(_enable_pin, OUTPUT);
  
  if (fourbitmode){
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	for (i = 0; i < 4; i++) {
		pinmode(_data_pins[i], OUTPUT);
		}
  }
  else {
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
    for (i = 0; i < 8; i++) {
		pinmode(_data_pins[i], OUTPUT);
	}
  }
  
}

/** LCD 8 bits mode */
void lcd(unsigned char rs, unsigned char enable, unsigned char d0, unsigned char d1, unsigned char d2, unsigned char d3, 
			unsigned char d4, unsigned char d5, unsigned char d6, unsigned char d7) {
	
	init(((d4 + d5 + d6 + d7)==0), rs, 99, enable, d0, d1, d2, d3, d4, d5, d6, d7);
	
}
#endif