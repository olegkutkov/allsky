/*
 Fancontrol daemon

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

char *readsysfs(char *dir, char *entry)
{
	char filename[PATH_MAX];
	char value[PAGE_SIZE];
	char *string = NULL;
	int fd;
	int len;

	if (dir == NULL)
		snprintf(filename, PATH_MAX, "%s/%s", MICSYSFSDIR, entry);
	else
		snprintf(filename, PATH_MAX,
			 "%s/%s/%s", MICSYSFSDIR, dir, entry);

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		mpsslog("Failed to open sysfs entry '%s': %s\n",
			filename, strerror(errno));
		return NULL;
	}

	len = read(fd, value, sizeof(value));
	if (len < 0) {
		mpsslog("Failed to read sysfs entry '%s': %s\n",
			filename, strerror(errno));
		goto readsys_ret;
	}
	if (len == 0)
		goto readsys_ret;

	value[len - 1] = '\0';

	string = malloc(strlen(value) + 1);
	if (string)
		strcpy(string, value);

readsys_ret:
	close(fd);
	return string;
}


