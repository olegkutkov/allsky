/*
 libsensors for allsky
 sensors defs

 Copyright 2017  Oleg Kutkov <elenbert@gmail.com>

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

#ifndef SENSORS_H
#define SENSORS_H

#define SENSORS_COUNT 5

#ifdef USE_DHT22
	#define DHT22_SENSOR_ID 0x0
	#define DHT22_DESCRIPTION \
		{ DHT22_SENSOR_ID, "DHT22", "Temperature and humidity sensor"},
#else
	#define DHT22_DESCRIPTION
#endif

#ifdef USE_TSL_2561
	#define TSL_2561_SENSOR_ID 0x1
	#define TSL_261_DESCRIPTION \
		{ TSL_2561_SENSOR_ID, "TSL 2561", "Luminosity/Lux/Light sensor" },
#else
	#define TSL_261_DESCRIPTION
#endif

#ifdef USE_MLX_90614
	#define MLX_90614_SENSOR_ID 0x2
	#define MLX_90614_DESCRIPTION \
		{ MLX_90614_SENSOR_ID, "MLX 90614", "Infrared thermometer" },
#else
	#define MLX_90614_DESCRIPTION
#endif

#ifdef USE_TACHO
	#define TACHOMETER_SENSOR_ID 0x3
	#define TACHO_DESCRIPTION \
		{ TACHOMETER_SENSOR_ID, "Tacho", "FAN's rpm sensor" },
#else
	#define TACHO_DESCRIPTION
#endif

#ifdef USE_RASPI_VCGEN
	#define RASPI_VCGEN_SENSOR_ID 0x4
	#define RASPI_VCGEN_DESCRIPTION \
		{ RASPI_VCGEN_SENSOR_ID, "Vcgen", "Raspberry pi temperature and voltages sensor" },
#else
	#define RASPI_VCGEN_DESCRIPTION
#endif

#define SENSORS_LIST_DESCRIPTION \
			{ \
				DHT22_DESCRIPTION \
				TSL_261_DESCRIPTION \
				MLX_90614_DESCRIPTION \
				TACHO_DESCRIPTION \
				RASPI_VCGEN_DESCRIPTION \
			}

#endif

