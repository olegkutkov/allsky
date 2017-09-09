/*
 libsensors for allsky
 main include

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

#ifndef LIBSENSORS_ALLSKY_H
#define LIBSENSORS_ALLSKY_H

#include "sensors.h"

#ifdef __cplusplus
extern “C” {
#endif

typedef struct sensor_info {
	int id;
	char* sensor_name;
	char* sensor_description;
} sensor_info_t;

typedef struct libsensors_allsky {
	int __semid;
	sensor_info_t* __sensors;
} libsensors_allsky_t;

libsensors_allsky_t* get_sensors(void);
void free_sensors(libsensors_allsky_t* sens);

sensor_info_t* get_sensors_list(libsensors_allsky_t* sens);

char* get_sensor_name(sensor_info_t* sinf);
char* get_sensor_description(sensor_info_t* sinf);

//void free_sensors_list(sensor_info_t* list);


/* private functions */

sensor_info_t* __build_sensors_list(void);

void __sema_lock_wait(const int semid, const int semnum);
void __sema_unlock(const int semid, const int semnum);

#ifdef __cplusplus
}
#endif

#endif 
