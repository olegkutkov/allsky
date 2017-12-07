/*
 Fancontrol daemon

 Copyright 2016  Oleg Kutkov <kutkov.o@yandex.ru>

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

#include <unistd.h>
#include <bcm2835.h>
#include "dht.h"

#define MAXTIMINGS 100

double readDHTTemp(int pin) {
	int counter = 0;
	int laststate = HIGH;
	int j = 0;

	int data[100];

	// Set GPIO pin to output
	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);

	// hold pin HIGH for 500 ms
	bcm2835_gpio_write(pin, HIGH);
	usleep(500000); 
	bcm2835_gpio_write(pin, LOW);
	usleep(20000);

	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	// wait for pin to drop?
	while (bcm2835_gpio_lev(pin) == 1) {
		usleep(1);
	}

	// read data!
	for (int i = 0; i < MAXTIMINGS; i++) {
		counter = 0;

		while (bcm2835_gpio_lev(pin) == laststate) {
			counter++;
			if (counter == 1000) {
	  			break;
			}
		}

		laststate = bcm2835_gpio_lev(pin);

		if (counter == 1000) {
			break;
		}

		if ((i > 3) && (i%2 == 0)) {
			// shove each bit into the storage bytes
			data[j/8] <<= 1;

			if (counter > 200) {
				data[j/8] |= 1;
			}

			j++;
		}
	}

	if ((j >= 39) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
		float f = (data[2] & 0x7F)* 256 + data[3];
		f /= 10.0;

		if (data[2] & 0x80) {
			f *= -1;
		}
 
		return f;
	}

	return READ_TEMP_FAILED;
}

