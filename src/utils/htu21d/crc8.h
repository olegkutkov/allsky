/*
 HTU21D sensor read using Raspberry i2c
  simple crc8 checker from the bbx10/htu21dflib github repo

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

#ifndef CRC8_H
#define CRC8_H

uint8_t crc8(uint8_t *p, uint8_t len);

#endif

