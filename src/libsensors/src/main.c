/*
 libsensors for allsky

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

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "libsensors_allsky.h"
#include "sensors.h"

#include <stdio.h>

libsensors_allsky_t* get_sensors()
{
	union semun {
		int val;
		struct semid_ds *buf;
		ushort *array;
	} sem_arg;

	int sem_id, i;
	key_t semkey;
	libsensors_allsky_t* sens;
	unsigned short sema_init_values[SENSORS_COUNT];

	if ((semkey = ftok("/opt/ipc_uniq_key", 'a')) == (key_t) -1) {
		return NULL;
	}

	if ((sem_id = semget(semkey, 0, 0)) == -1) {
		/* Request SENSORS_COUNT semas. Sync objects for each sensor */
		if ((sem_id = semget(semkey, SENSORS_COUNT, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
			return NULL;
		} else {
			for (i = 0; i < SENSORS_COUNT; i++) {
				sema_init_values[i] = 1;
			}

			sem_arg.array = sema_init_values;

			if (semctl(sem_id, 0, SETALL, sem_arg) < 0) {
				return NULL;
			}
		}
	}

	sens = (libsensors_allsky_t*) malloc(sizeof(libsensors_allsky_t));

	if (!sens) {
		return NULL;
	}

	sens->__semid = sem_id;
	sens->__sensors = __build_sensors_list();

	return sens;
}

void free_sensors(libsensors_allsky_t* sens)
{
	if (sens) {
		if (sens->__sensors) {
			free(sens->__sensors);
			sens->__sensors = NULL;
		}

		free(sens);
		sens = NULL;
	}
}

char* get_sensor_name(sensor_info_t* sinf)
{
	return sinf->sensor_name;
}

char* get_sensor_description(sensor_info_t* sinf)
{
	return sinf->sensor_description;
}

sensor_info_t* __build_sensors_list(void)
{
	int i;
	sensor_info_t sens_tmp[SENSORS_COUNT] = SENSORS_LIST_DESCRIPTION;
	sensor_info_t* sensors_list = (sensor_info_t*) malloc(sizeof(sensor_info_t) * SENSORS_COUNT);

	memcpy(sensors_list, &sens_tmp, sizeof(sensor_info_t) * SENSORS_COUNT);

	return sensors_list;
}

void __sema_lock_wait(const int semid, const int semnum)
{
	struct sembuf op[1];

	op[0].sem_num = semnum;

	/* Decrement by 1.  */
	op[0].sem_op = -1;

	/* Permit undo’ing.  */
	op[0].sem_flg = SEM_UNDO;

	semop(semid, op, 1);
}

void __sema_unlock(const int semid, const int semnum)
{
	struct sembuf op[1];

	op[0].sem_num = semnum;

	/* Increment by 1.  */
	op[0].sem_op = 1;

	/* Permit undo’ing.  */
	op[0].sem_flg = SEM_UNDO;

	semop(semid, op, 1);
}

sensor_info_t* get_sensors_list(libsensors_allsky_t* sens)
{
	return sens->__sensors;

//	sensor_info_t* sensors_list = (sensor_info_t*) malloc(sizeof(sensor_info_t));

//	return sensors_list;
}

//void free_sensors_list(sensor_info_t* list)
//{
//	free(list);
//}

