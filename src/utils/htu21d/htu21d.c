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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include "htu21d.h"
#include "crc8.h"

static int DEBUG_MODE = 0;
static int NO_HOLD_MASTER_MODE = 1;

extern const char* __progname;

int get_device(const int bus_num, const unsigned char i2c_addr)
{
	char dev_path[11] = { 0 };

	// construct path to i2c device
	snprintf(dev_path, 11, "/dev/i2c-%i", bus_num);

	if (DEBUG_MODE) {
		fprintf(stderr, "Opening i2c interface %s\n", dev_path);
	}

	int fdev = open(dev_path, O_RDWR);

	if (fdev < 0) {
		fprintf(stderr, "Failed to open I2C interface %s Error: %s\n", dev_path, strerror(errno));
		return -1;
	}

	if (DEBUG_MODE) {
		fprintf(stderr, "Setting up slave address 0x%02X\n", i2c_addr);
	}

	// set addr of the slave i2c device
	if (ioctl(fdev, I2C_SLAVE, i2c_addr) < 0) {
		fprintf(stderr, "Failed to select I2C slave device! Error: %s\n", strerror(errno));
		return -1;
	}

	return fdev;
}

int send_device_cmd(const int fdev, const char cmd)
{
	uint8_t buf[1];

	buf[0] = cmd;

	if (DEBUG_MODE) {
		fprintf(stderr, "Sending command: 0x%.2X\n", cmd);
	}

	return write(fdev, buf, 1) == 1;
}

unsigned int read_from_device(const int fdev)
{
	uint8_t buf[3] = { 0 };
	int counter = 0;

	if (DEBUG_MODE) {
		fprintf(stderr, "Reading sensor data in a %s mode\n"
						, (NO_HOLD_MASTER_MODE ? "no hold master" : "hold master"));
	}

	while (1) {
		// in no hold mode read() blocked till measurements finished
		// so additional sleep is not required
		if (NO_HOLD_MASTER_MODE) {
			usleep(MEASURE_WAIT_US);
			counter++;
		}

		if (read(fdev, buf, 3) != 3) {
			if (DEBUG_MODE) {
				printf("Read err: %s, retrying!\n", strerror(errno));
			}

			if (NO_HOLD_MASTER_MODE) {
				if (counter >= MAX_READ_RETRY_COUNT) {
					break;
				}

				continue;
			}
		}

		break;
	}

	// device response, 14-bit ADC value:
	//  first 8 bit part ACK  second 8 bit part        CRC
	// [0 1 2 3 4 5 6 7] [8] [9 10 11 12 13 14 15 16] [17 18 19 20 21 22 23 24]
	// bit 15 - measurement type (‘0’: temperature, ‘1’: humidity)
	// bit 16 - currently not assigned

	if (crc8(buf, 3) != 0) {
		fprintf(stderr, "CRC failed!\n");
		return 0;
	}

	// combine two 8 bit number in to one 16 bit + skip two last least significant bits
	return (buf [0] << 8 | buf [1]) & 0xFFFC;
}

double calc_temperature_c(unsigned int data)
{
	double sensor_temp = data / 65536.0;

	// check out the HTU21D datasheet, page 15
	// Temperature conversion

	return -46.85 + (175.72 * sensor_temp);
}

double calc_humidity(unsigned int data)
{
	double sensor_humid = data / 65536.0;

	// check out the HTU21D datasheet, page 15
	// Relative Humidity conversion

	return -6.0 + (125.0 * sensor_humid);
}

void read_data_from_sensor(const int fdev, int pretty)
{
	if (DEBUG_MODE) {
		fprintf(stderr, "Doing soft reset of the device\n");
	}

	send_device_cmd(fdev, HTU21_SOFT_RESET); // Soft Reset

	usleep(STARTUP_WAIT_US);

	if (DEBUG_MODE) {
		fprintf(stderr, "Triggering temperature measuerement\n");
	}

	// Trigger Temperature Measurement
	send_device_cmd(fdev, (NO_HOLD_MASTER_MODE 
							? HTU21_READ_TEMP_NH : HTU21_READ_TEMP_H)); 
	
	unsigned int res = read_from_device(fdev);

	if (res == 0) {
		fprintf(stderr, "Failed to read from the device, exiting...\n");
		return;
	}

	double temp_c = calc_temperature_c(res);


	if (DEBUG_MODE) {
		fprintf(stderr, "Triggering humidity measuerement\n");
	}

	// Trigger Humidity Measurement
	send_device_cmd(fdev, (NO_HOLD_MASTER_MODE
							? HTU21_READ_HUMID_NH : HTU21_READ_HUMID_H)); 

	res = read_from_device(fdev);

	double humid = calc_humidity(res);

	if (pretty) {
		printf("Temperature: %.2f C\n", temp_c);
		printf("Humidity: %.2f %%\n", humid);
	} else {
		printf("temp=%.2f humidity=%.2f\n", temp_c, humid);
	}
}

void show_usage()
{
	printf("Usage\n");
	printf("\t%s --bus [0-1] --i2c_addr [0x00-0x7F] options", __progname);
	printf("\n");
	printf("\t\t-b, --bus\t\t- set i2c bus number (0 for Raspbery PI model A, 1 for Raspberry PI model B, default is 0)\n");
	printf("\t\t-c, --i2c_addr\t\t- set slave device address (default = 0x40)\n");
	printf("\t\t-l, --hod\t\t- hoLd the master mode\n");
	printf("\t\t-p, --pretty\t\t- more pretty output\n");
	printf("\t\t-d, --debug\t\t- show debug output\n");
}

int main(int argc, char **argv)
{
	int bus_num = 1;
	unsigned char i2c_addr = HTU21_I2CADDR;
	int pretty_out = 0;

	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "bus", required_argument, NULL, 'b' },
		{ "i2c_addr", required_argument, NULL, 'c' },
		{ "hold", no_argument, NULL, 'l' },
		{ "pretty", no_argument, NULL, 'p'},
		{ "debug", no_argument, NULL, 'd' }
	};

	int option_index = 0;
	int opt = getopt_long(argc, argv, "hb:c:lpd", long_options, &option_index);

	while (opt != -1) {
		switch (opt) {
			case 'h':
				show_usage();
				return 0;

			case 'b':
				bus_num = atoi(optarg);
				break;

			case 'c':
				i2c_addr = strtol(optarg, NULL, 64);
				break;

			case 'l':
				NO_HOLD_MASTER_MODE = 0;
				break;

			case 'p':
				pretty_out = 1;
				break;

			case 'd':
				DEBUG_MODE = 1;
				break;

			default:
				show_usage();
				abort();
		}

		opt = getopt_long(argc, argv, "b:c:lpd", long_options, &option_index);
	}

	int fdev = get_device(bus_num, i2c_addr);

	if (fdev < 0) {
		fprintf(stderr, "Failed to open device 0x%X on bus %i\n", i2c_addr, bus_num);
		return -1;
	}

	read_data_from_sensor(fdev, pretty_out);

	close(fdev);
}

