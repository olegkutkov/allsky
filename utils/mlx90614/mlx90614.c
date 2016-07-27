/*
 MLX90614 sensor read from Raspberyy I2C

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
#include "mlx_addrs.h"

// buffer for data reading or writing
typedef union i2c_smbus_data i2c_data;


static int DEBUG_MODE = 0;
///

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

	// enable checksums
	if (ioctl(fdev, I2C_PEC, 1) < 0) {
		fprintf(stderr, "Failed to enable SMBus packet error checking, error: %s\n", strerror(errno));
		return -1;
	}

	return fdev;
}

int talk_to_device(const int fdev, const int read, const char command, i2c_data* data)
{
	// initialize i2c_smus structure for combined write/read request to device
	struct i2c_smbus_ioctl_data sdat = {
		.read_write = (read ? I2C_SMBUS_READ : I2C_SMBUS_WRITE), // set operation type: read or write
		.command = command,		// set command, i.e. register number
		.size = I2C_SMBUS_WORD_DATA,   // set data size, note: mlx supports only WORD
		.data = data    // pointer to data
	};

	if (DEBUG_MODE) {
		fprintf(stderr, "Perfoming %s request to device, command = 0x%02X\n"
						, (read ? "I2C_SMBUS_READ" : "I2C_SMBUS_WRITE"), command);
	}

	// perfom combined request to device
	if (ioctl(fdev, I2C_SMBUS, &sdat) < 0) {
		fprintf(stderr, "Failed to perfom I2C_SMBUS transaction, error: %s\n", strerror(errno));
		return -1;
	}

	if (DEBUG_MODE) {
		fprintf(stderr, "Ok, got answer from device\n");
	}

	return 0;
}

int check_args(const int bus_num, const unsigned char i2c_addr)
{
	if (bus_num > 1 || bus_num < 0) {
		fprintf(stderr, "Invalid bus number %i, please select 0 or 1\n", bus_num);
		return -1;
	}

	if (i2c_addr == 0) {
		fprintf(stderr, "Invalid i2c device address, please set proper address of the MLX\n");
		return -1;
	}

	return 0;
}

int read_data_from_sensor(const int fdev, const char command)
{
	i2c_data data;

	if (talk_to_device(fdev, 1, command, &data) < 0) {
		return  -1;
	}

	double temp = 0;

	switch (command) {
		case MLX90614_TA:
		case MLX90614_TOBJ1:
		case MLX90614_TOBJ2:
			temp = (double) data.word;
			temp = (temp * 0.02)-0.01;
			temp = temp - 273.15;
			printf("%s = %04.2f\n", (command == MLX90614_TA ? "Tamb" : "Tobj"), temp);

			break;

		case MLX90614_EMISS:
			printf("Emissivity correction coefficient = %i\n", data.word);
			break;

		case MLX90614_PWMCTRL:
			if (!(data.word & (1 << 1))) {
				printf("PWM mode - disabled\n");
			} else {
				printf("PWM mode - enabled\n");
				printf("In order to disable pwm mode - pull down SCL for >1 microsecond and change EEPROM setting.\n");
			}

			break;
	}

	return 0;
}

int write_data_to_sensor(const int fdev, const char command, const unsigned short write_arg)
{
	i2c_data msg;

	msg.word = 0x0;

	if (DEBUG_MODE) {
		fprintf(stderr, "Erasing EEPROM cell = 0x%02X\n", command);
	}

	if (talk_to_device(fdev, 0, command, &msg) < 0) {
		fprintf(stderr, "Unable to erase EEPROM cell\n");
		return -1;
	}

	// delay between eeprom erasing and writing new value
    // without this delay writing to device may fail
	usleep(5000);

	if (command == MLX90614_ADDR) {
		msg.word = 0xFFFF;
		msg.word = msg.word << 8 | write_arg;
	} else {
		msg.word = write_arg;
	}

	if (DEBUG_MODE) {
		fprintf(stderr, "Trying to store value = 0x%04X to the EEPROM cell = 0x%02X\n", msg.word, command);
	}

	if (talk_to_device(fdev, 0, command, &msg) < 0) {
		fprintf(stderr, "Unable to write to EEPROM\n");
		return -1;
	}

	usleep(5000);

	if (command == MLX90614_ADDR) {
		printf("MLX device address succesfully changed to 0x%X\n", msg.word);
		printf("Please, power off and power on again the device to apply changes\n");
	} else if (command == MLX90614_EMISS) {
		printf("Warning! Emissivity correction coefficient was changed to %i\n", msg.word);
	}

	return 0;
}

int main(int argc, char **argv)
{
	int bus_num = 0;
	unsigned char i2c_addr = MLX90614_I2CADDR;

	int op_read = 1;
	int write_arg_set = 0;

	unsigned char command = 0x00;
	unsigned short write_arg = 0x00;

	static struct option long_options[] = {
		{ "bus", required_argument, NULL, 'b' },
		{ "i2c_addr", required_argument, NULL, 'c' },
		{ "new_addr", required_argument, NULL, 'r'},
		{ "write", no_argument, NULL, 'w' },
		{ "get_ir_temp", no_argument, NULL, 'i' },
		{ "get_ambient_temp", no_argument, NULL, 'a' },
		{ "emissivity_coefficient", optional_argument, NULL, 'e'},
		{ "pwm_mode", optional_argument, NULL,'p'},
		{ "debug", no_argument, NULL, 'd' }
	};

	int option_index = 0;
	int opt = getopt_long(argc, argv, "bc:r:wiae:p:d", long_options, &option_index);

	while (opt != -1) {
		switch (opt) {
			case 'b':
				bus_num = atoi(optarg);
				break;

			case 'c':
				i2c_addr = strtol(optarg, NULL, 16);
				break;

			case 'r':
				write_arg = strtol(optarg, NULL, 16);;
				write_arg_set = 1;
				command = MLX90614_ADDR;
				break;

			case 'w':
				op_read = 0;
				break;

			case 'i':
				command = MLX90614_TOBJ1;
				break;

			case 'a':
				command = MLX90614_TA;
				break;

			case 'e':
				command = MLX90614_EMISS;

				if (optarg) {
					write_arg = atoi(optarg);
					printf("%i\n", write_arg);
					write_arg_set = 1;
				}

				break;

			case 'p':
				command = MLX90614_PWMCTRL;

				if (optarg) {
					write_arg = atoi(optarg);
					write_arg_set = 1;
				}

				break;

			case 'd':
				DEBUG_MODE = 1;
				break;

			default:
				abort();
		}

		opt = getopt_long(argc, argv, "bc:wiaep", long_options, &option_index);
	}

	if (check_args(bus_num, i2c_addr) < 0) {
		return -1;
	}

	if (!op_read && (command == MLX90614_TOBJ1 || command == MLX90614_TA)) {
		fprintf(stderr, "Read only data!\n");
		return -1;
	}

	if (!op_read && !write_arg_set) {
		fprintf(stderr, "Plese set parameter value for writing\n");
		return -1;
	}

	int fdev = get_device(bus_num, i2c_addr);

	if (fdev < 0) {
		return -1;
	}

	int res;

	if (op_read) {
		res = read_data_from_sensor(fdev, command);
	} else {
		res = write_data_to_sensor(fdev, command, write_arg);
	}

	close(fdev);

	return res;
}

