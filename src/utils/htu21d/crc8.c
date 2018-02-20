/*
 HTU21D sensor read using Raspberry i2c
  simple crc8 checker from the bbx10/htu21dflib github repo

 Copyright (c) 2014 bbx10node@gmail.com
 Copyright 2018  Oleg Kutkov <elenbert@gmail.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.  
*/

#include <stddef.h>
#include <stdint.h>

int crc8(const uint8_t *buf, int len)
{
	uint32_t dataandcrc;

	// Generator polynomial: x**8 + x**5 + x**4 + 1 = 1001 1000 1
	const uint32_t poly = 0x98800000;
	int i;

	if (len != 3) {
		return -1;
	}

    if (buf == NULL) {
		return -1;
	}

	// Justify the data on the MSB side. Note the poly is also
	// justified the same way.
	dataandcrc = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8);
 
	for (i = 0; i < 24; i++) {
		if (dataandcrc & 0x80000000UL) {
			dataandcrc ^= poly;
		}

		dataandcrc <<= 1;
	}

	return (dataandcrc != 0);
}

