/*
 HTU21D sensor read using Raspberry i2c

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

#ifndef HTU21D_H
#define HTU21D_H

#define HTU21_I2CADDR 0x40

#define HTU21_SOFT_RESET 0xFE
#define HTU21_READ_TEMP_NH 0xF3
#define HTU21_READ_HUMID_NH 0xF5
#define HTU21_READ_TEMP_H 0xE3
#define HTU21_READ_HUMID_H 0xE5

#define STARTUP_WAIT_US 15000
#define MEASURE_WAIT_US 50000

#define MAX_READ_RETRY_COUNT 5

#endif

