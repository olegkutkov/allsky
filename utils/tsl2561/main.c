/*
 TSL2561 sensor read from Raspberyy GPIO

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
#include <string.h>
#include "TSL2561.h"

int main() {
	int rc;
	uint16_t broadband, ir;
	uint32_t lux=0;

	// prepare the sensor
	// (the first parameter is the raspberry pi i2c master controller attached to the TSL2561, the second is the i2c selection jumper)
	// The i2c selection address can be one of: TSL2561_ADDR_LOW, TSL2561_ADDR_FLOAT or TSL2561_ADDR_HIGH
	TSL2561 light1 = TSL2561_INIT(1, TSL2561_ADDR_FLOAT);
	
	// initialize the sensor
	rc = TSL2561_OPEN(&light1);

	if(rc != 0) {
		fprintf(stderr, "Error initializing TSL2561 sensor (%s). Check your i2c bus (es. i2cdetect)\n", strerror(light1.lasterr));
		// you don't need to TSL2561_CLOSE() if TSL2561_OPEN() failed, but it's safe doing it.
		TSL2561_CLOSE(&light1);
		return 1;
	}
	
	// set the gain to 1X (it can be TSL2561_GAIN_1X or TSL2561_GAIN_16X)
	// use 16X gain to get more precision in dark ambients, or enable auto gain below
	rc = TSL2561_SETGAIN(&light1, TSL2561_GAIN_1X);
	
	// set the integration time 
	// (TSL2561_INTEGRATIONTIME_402MS or TSL2561_INTEGRATIONTIME_101MS or TSL2561_INTEGRATIONTIME_13MS)
	// TSL2561_INTEGRATIONTIME_402MS is slower but more precise, TSL2561_INTEGRATIONTIME_13MS is very fast but not so precise
	rc = TSL2561_SETINTEGRATIONTIME(&light1, TSL2561_INTEGRATIONTIME_402MS);
	
	// sense the luminosity from the sensor (lux is the luminosity taken in "lux" measure units)
	// the last parameter can be 1 to enable library auto gain, or 0 to disable it
	rc = TSL2561_SENSELIGHT(&light1, &broadband, &ir, &lux, 1);

	if (rc == 0) {
		printf("broadband=%i, infrared=%i, lux=%i\n", broadband, ir, lux);
	} else {
		fprintf(stderr, "Sensor read error: %i(%s)\n", rc, strerror(light1.lasterr));
	}
	
	TSL2561_CLOSE(&light1);
	
	return 0;
}
