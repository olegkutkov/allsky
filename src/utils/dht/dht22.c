/*
 DHT-22 sensor read from Raspberyy GPIO

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

#define BCM2708_PERI_BASE	0x20000000
#define GPIO_BASE	(BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <bcm2835.h>
#include <unistd.h>

#define MAXTIMINGS 100

int bits[250], data[100];
int bitidx = 0;

int readDHT(int pin);

int main(int argc, char **argv)
{
	if (!bcm2835_init()) {
		return -1;
	}

	if (argc != 2) {
		printf("usage: %s GPIOpin#\n", argv[0]);
		return -1;
	}
  
	int dhtpin = atoi(argv[1]);

	if (dhtpin <= 0) {
		printf("Please select a valid GPIO pin #\n");
		return -1;
	}

	int try = 0, maxtry = 3;

	while (try < maxtry) {
		if (readDHT(dhtpin)) {
			break;
		}

		sleep(3);
		try++;
	}

	return 0;
}

int readDHT(int pin) {
	int counter = 0;
	int laststate = HIGH;
	int j = 0;

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
		float h = data[0] * 256 + data[1];
		h /= 10;

		float f = (data[2] & 0x7F)* 256 + data[3];
		f /= 10.0;

		if (data[2] & 0x80) {
			f *= -1;
		}

		// is some magic cases sensor returns very strange data (line T > 1000)
		if (f > 80 || f < -40 || h < 0 || h > 100) {
			return 0;
		}

		// yet another strange case
		if (f == 0 && h == 0) {
			return 0;
		}

		printf("temp=%.1f humidity=%.1f\n", f, h);
    
		return 1;
	}

	return 0;
}

