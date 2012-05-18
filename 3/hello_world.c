/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <exports.h>

int hello_world (int argc, char *argv[]) {

	//Create address pointers for userkey, led and led output-enable
	volatile unsigned long *USERKEY = 0x48310038;
	volatile unsigned long *LED = 0x4905803c
	volatile unsigned long *LEDOE = 0x49058034;

	//Set LED direction to output
	*LEDOE &= 0xfffffff7;

	printf ("Activate USER_KEY to toggle LED \n");
	
	while(1) {

		if((*USERKEY & 0x04000000) == 0)
			*LED |= 0x00000008;
		
		else 
			*LED &= 0xfffffff7;
	}

	return 0;
}
	
