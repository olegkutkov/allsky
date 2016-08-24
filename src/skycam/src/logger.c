/*
 Copyright 2012  Oleg Kutkov <kutkov.o@yandex.ru>

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

#include <syslog.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include "logger.h"
//#include <program.h>

extern const char* __progname;

struct logger_t {
	int max_log_level;
	int use_stdout;
	FILE* out_file;
	void (*logger_func) (const int level, const char*);
};

#define PROGRAM_NAME __progname

#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_STATUS 2
#define LOG_LEVEL_DEBUG 3

#define LOG_PREFIX_ERROR "ERROR"
#define LOG_PREFIX_WARNING "WARNING"
#define LOG_PREFIX_STATUS "STATUS"
#define LOG_PREFIX_DEBUG "DEBUG"

static struct logger_t log_global_set;
static const char* LOG_LEVELS[] = { LOG_PREFIX_ERROR,
				    LOG_PREFIX_WARNING,
				    LOG_PREFIX_STATUS,
				    LOG_PREFIX_DEBUG };

void print_to_syslog(const int level, const char* message);
void print_to_file(const int level, const char* message);

void cleanup_internal()
{
	if (log_global_set.out_file) {
		if (!log_global_set.use_stdout) {
			fclose(log_global_set.out_file);
		}

		log_global_set.use_stdout = 0;
		log_global_set.out_file = NULL;
	}
}

void logger_reset_state(void)
{
	log_global_set.max_log_level = LOG_MAX_LEVEL_ERROR_WARNING_STATUS;
	cleanup_internal();
	log_global_set.logger_func = print_to_syslog;
}

void print_to_syslog(const int level, const char* message)
{
	syslog(LOG_INFO, "[%s] %s\n", LOG_LEVELS[level], message);
}

void print_to_file(const int level, const char* message)
{
	struct tm* current_tm;
	time_t time_now;

	time(&time_now);
	current_tm = localtime(&time_now);

	int res = fprintf(log_global_set.out_file,
			"%s: %02i:%02i:%02i [%s] %s\n"
				, PROGRAM_NAME
				, current_tm->tm_hour
				, current_tm->tm_min
				, current_tm->tm_sec
				, LOG_LEVELS[level]
				, message );

	if (res == -1) {
		print_to_syslog(LOG_LEVEL_ERROR, "Unable to write to log file!");
		return;
	}

	fflush(log_global_set.out_file);
}

void logger_critical_log(const char* message)
{
	print_to_syslog(LOG_LEVEL_ERROR, message);
}

void logger_set_log_level(const int level)
{
	log_global_set.max_log_level = level;
}

int logger_set_log_file(const char* filename)
{
	cleanup_internal();

	log_global_set.out_file = fopen(filename, "a");

	if (log_global_set.out_file == NULL) {
		log_error("Failed to open file %s error %s", filename, strerror(errno));
		return -1;
	}

	log_global_set.logger_func = print_to_file;

	return 0;
}

void logger_set_out_stdout()
{
	cleanup_internal();

	log_global_set.use_stdout = 1;
	log_global_set.logger_func = print_to_file;
	log_global_set.out_file = stdout;
}

void log_generic(const int level, const char* format, va_list args)
{
	char buffer[256];
	vsprintf(buffer, format, args);
	log_global_set.logger_func(level, buffer);
}

void log_error(char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_generic(LOG_LEVEL_ERROR, format, args);
	va_end(args);
}

void log_warning(char *format, ...)
{
	if (log_global_set.max_log_level < LOG_MAX_LEVEL_ERROR_WARNING_STATUS) {
		return;
	}

	va_list args;
	va_start(args, format);
	log_generic(LOG_LEVEL_WARNING, format, args);
	va_end(args);
}

void log_status(char *format, ...)
{
	if (log_global_set.max_log_level < LOG_MAX_LEVEL_ERROR_WARNING_STATUS) {
		return;
	}

	va_list args;
	va_start(args, format);
	log_generic(LOG_LEVEL_STATUS, format, args);
	va_end(args);
}

void log_debug(char *format, ...)
{
	if (log_global_set.max_log_level <  LOG_MAX_LEVEL_ERROR_WARNING_STATUS_DEBUG) {
		return;
	}

	va_list args;
	va_start(args, format);
	log_generic(LOG_LEVEL_DEBUG, format, args);
	va_end(args);
}


