
#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#include "dht.h"

#define PIN RPI_GPIO_P1_12

void gentle_set_fan_speed(const int last_speed, const int new_speed)
{
	int slow_down, count, val = last_speed;

	if (last_speed > new_speed) {
		slow_down = 1;
		count = last_speed - new_speed;
	} else {
		slow_down = 0;
		count = new_speed - last_speed;
	}

	for (int i = 0; i < count; i++) {
		if (slow_down) {
			val--;
			bcm2835_pwm_set_data(0, val);
		} else {
			val++;
			bcm2835_pwm_set_data(0, val);
		}

		bcm2835_delay(6);
	}
}

double get_temperature(int sensor_pin)
{
	int try = 0, maxtry = 5;
	double temp = 0;

	while (try < maxtry) {
		temp = readDHTTemp(sensor_pin);

		if (temp != READ_TEMP_FAILED) {
			break;
			sleep(5);
			try++;
		}
	}

	return temp;
}

#define GET_INTERNAL_TEMPERATURE get_temperature(INTERNAL_SENSOR_PIN)
#define GET_EXTERNAL_TEMPERATURE get_temperature(EXTERNAL_SENSOR_PIN)

int main()
{
	bcm2835_init();
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5);
	bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_64);
	bcm2835_pwm_set_mode(0, 1, 1);
	bcm2835_pwm_set_range(0, 1024);

	int optimal_already_set = 0;

	gentle_set_fan_speed(20, 5);

//	sleep(3);

//	while (1) {
		double external_temp = GET_EXTERNAL_TEMPERATURE;

		if (external_temp <= 10.0) {    // autumn or winter, just enable fans at ~40% of max speed to warm up box and don't let fans to freeze
			printf("Winter is coming! Set fan PWM to 500\n");
			gentle_set_fan_speed(1024, 500);
//			sleep(10);
//			continue;
		}

		double internal_temp = GET_INTERNAL_TEMPERATURE;

		printf("Internal temp=%.1f\n", internal_temp);

		if (internal_temp >= 36.0 && internal_temp < 42.0) {
			printf("Internal temperature is >= 36 and < 42 - spinning from 20 to 1050\n");

			optimal_already_set = 0;

			gentle_set_fan_speed(900, 20);
			sleep(10);
			gentle_set_fan_speed(20, 40);
			sleep(5);
			gentle_set_fan_speed(40, 60);
			sleep(5);
			gentle_set_fan_speed(60, 90);
//			sleep(15);
//			continue;
		} else if (internal_temp >= 42.0) {
			printf("Internal temperature is >= 42 - spinning from 5 to 650\n");

			optimal_already_set = 0;

			gentle_set_fan_speed(900, 5);
			sleep(15);
			gentle_set_fan_speed(5, 40);
			sleep(5);
			gentle_set_fan_speed(40, 100);
			sleep(3);
			gentle_set_fan_speed(100, 650);
//			sleep(30);
//			continue;
		} else if (internal_temp <= 28.0 && internal_temp >= 20.0) {
			printf("Internal temperature is <= 28 and >= 20 - slow slow down fans\n");
			gentle_set_fan_speed(1024, 1023);
		} else {
			printf("Internal temperature is optimal - just spin fans at 650\n");
			gentle_set_fan_speed(60, 650);
			optimal_already_set = 1;
		}

//		sleep(20);
//	}

	bcm2835_close();

	return 0;
}

