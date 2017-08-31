
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libsensors_allsky/libsensors_allsky.h>

int main()
{
	libsensors_allsky_t* sens = get_sensors();

	if (!sens) {
		printf("Failed to get_sensors, error: %s\n", strerror(errno));
		return -1;
	}

	printf("Locking library...\n");

	lock_lib(sens);

	printf("Library locked\n");

	sleep(5);

	printf("Unlocking library...\n");

	unlock_lib(sens);

	printf("Ok\n");

	free_sensors(sens);

	return 0;
}


