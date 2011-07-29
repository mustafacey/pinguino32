/*-------------------------------------------------------------------------
  main32.c - Application main function for Pinguino 32

             (c) 2010, 2011 Jean-Pierre Mandon <jp.mandon@gmail.com>
             (c) 2010, 2011 Régis Blanchot <rblanchot@gmail.com> 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-------------------------------------------------------------------------*/

#include <p32xxxx.h>					// always in first place to avoid conflict with const.h ON
#include <typedef.h>
#include <const.h>
#include <macro.h>
#include <system.c>
#include "define.h"
#include <newlib.c>

#include "user.c"

int main()
{
	#ifdef PIC32_PINGUINO
	TRISDbits.TRISD9=1;			// because PORTB is shared with SDA on Olimex board
	TRISDbits.TRISD10=1;			// because PORTB is shared with SCL on Olimex board
	#endif	

	SystemConfig(80000000);			// default clock frequency is 80Mhz
	AD1PCFG = 0xFFFF;					// All pins of PORTB as digital IOs

	#ifdef __ANALOG__
	analog_init();
	#endif

	#ifdef __MILLIS__
	millis_init();
	#endif

	#ifdef __PWM__
	PWM_init();
	#endif    

	#ifdef __USBCDC
	CDC_init();
	#endif    

	#ifdef __RTCC_C
	RTCC_init();
	#endif    
	
	setup();

	while (1)
	{
		#ifdef __USBCDC
		CDCTxService();
		#endif    
		loop();
	}

	return(0);    
}

#ifndef __SERIAL_C
void Serial1Interrupt(void)
{
	Nop();    
}

void Serial2Interrupt(void)
{
	Nop();    
}
#endif

#ifndef __MILLIS__
void Tmr2Interrupt(void)
{
	Nop();    
}
#endif

#ifndef __SPI_C
void SPI1Interrupt(void)
{
	Nop();    
}

void SPI2Interrupt(void)
{
	Nop();    
}
#endif /* __SPI_C */

// vector 35
#ifndef __RTCC_C
void RTCCInterrupt(void)
{
	Nop();    
}
#endif /* __RTCC_C */

