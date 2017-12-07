/*
 Simple program for IRIS control

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

#include <stdio.h>
#include <bcm2835.h>
#include <unistd.h>

#define IRIS_PIN RPI_GPIO_P1_16

int main(int argc, char **argv)
{
	if (argc == 1) {
		return 1;
	}

	if (!bcm2835_init()) {
		fprintf(stderr, "Failed to initialize bcm2835\n");
		return 1;
	}

	bcm2835_gpio_fsel(IRIS_PIN, BCM2835_GPIO_FSEL_OUTP);

	char arg = *argv[1];

	switch (arg) {
		case 'o':
			printf("IRIS - open\n");
			bcm2835_gpio_write(IRIS_PIN, LOW);
			break;

		case 'c':
			printf("IRIS - close\n");
			bcm2835_gpio_write(IRIS_PIN, HIGH);
			break;

		default:
			break;
	}

	bcm2835_close();

	return 0;
}

