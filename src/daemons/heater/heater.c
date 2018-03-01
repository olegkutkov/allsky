#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <bcm2835.h>

#define HEATER_CHANNEL_1 RPI_GPIO_P1_26
#define HEATER_CHANNEL_2 RPI_GPIO_P1_24
#define HEATER_CHANNEL_3 RPI_GPIO_P1_22

double read_device(char* devid)
{
	char path[] = "/sys/bus/w1/devices";
	char devPath[128];
	char buf[256];
	char tmpData[6];
	ssize_t numRead;
	double temperatureC;

	sprintf(devPath, "%s/%s/w1_slave", path, devid);

	int fd = open(devPath, O_RDONLY);

	if(fd == -1) {
		perror ("Couldn't open the w1 device.");
		return -1;
	}

	while((numRead = read(fd, buf, 256)) > 0)  {
		strncpy(tmpData, strstr(buf, "t=") + 2, 5);

		temperatureC = strtof(tmpData, NULL) / 1000;

		printf("Temp: %.3f C\n", temperatureC);

		break;
	}

	close(fd);

	return temperatureC;
}

int main()
{
	if (!bcm2835_init()) {
		fprintf(stderr, "Failed to initialize bcm2835\n");
		return 1;
	}

	bcm2835_gpio_fsel(HEATER_CHANNEL_1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(HEATER_CHANNEL_2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(HEATER_CHANNEL_3, BCM2835_GPIO_FSEL_OUTP);

//	bcm2835_gpio_write(HEATER_CHANNEL_1, HIGH);

//	sleep(10);

//	bcm2835_gpio_write(HEATER_CHANNEL_1, LOW);


	while (1) {
//		double value = read_device("28-0000064d0b68");
		double value = read_device("28-0000064e24a3");

		if (value < 45.0) {
			printf("Enable heater\n");
			bcm2835_gpio_write(HEATER_CHANNEL_3, HIGH);
		} else {
			printf("Disable heater\n");
			bcm2835_gpio_write(HEATER_CHANNEL_3, LOW);
		}

		usleep(5000);
	}


	return 0;
}

