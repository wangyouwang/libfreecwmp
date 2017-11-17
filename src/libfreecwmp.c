/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
 */

#define _GNU_SOURCE

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <string.h>

#include "libfreecwmp.h"

static const int log_class[] = {
	[L_CRIT] = LOG_CRIT,
	[L_WARNING] = LOG_WARNING,
	[L_NOTICE] = LOG_NOTICE,
	[L_INFO] = LOG_INFO,
	[L_DEBUG] = LOG_DEBUG
};

void lfc_log_message(char *name, int priority, const char *format, ...)
{
	va_list vl;

	openlog(name, 0, LOG_DAEMON);

	va_start(vl, format);
	vsyslog(log_class[priority], format, vl);
	va_end(vl);

	closelog();
}

char * lfc_str_action(int code)
{
	switch (code) {
		case GET_PARAMETER_VALUE:
				return "get parameter value";
		case SET_PARAMETER_VALUE:
				return "set parameter value";
		case GET_PARAMETER_NOTIFICATION:
				return "get parameter notification";
		case SET_PARAMETER_NOTIFICATION:
				return "set parameter notification";
		default:
			break;
	}
	return NULL;
}

char * lfc_str_event_code(int code, bool human)
{
	switch (code) {
		case BOOTSTRAP:
			if (human)
				return "bootstrap";
			else
				return "0 BOOTSTRAP";
		case BOOT:
			if (human)
				return "boot";
			else
				return "1 BOOT";
		case PERIODIC:
			if (human)
				return "periodic";
			else
				return "2 PERIODIC";
		case SCHEDULED:
			if (human)
				return "scheduled";
			else
				return "3 SCHEDULED";
		case VALUE_CHANGE:
			if (human)
				return "value_change";
			else
				return "4 VALUE CHANGE";
		case CONNECTION_REQUEST:
			if (human)
				return "connection_request";
			else
				return "6 CONNECTION REQUEST";
		default:
			break;
	}
	return NULL;
}

int lfc_int_event_code(char *code)
{
	if (!strcasecmp("boot", code) ||
	    !strcasecmp("1 boot", code))
		return BOOT;

	if (!strcasecmp("periodic", code) ||
	    !strcasecmp("2 periodic", code))
		return PERIODIC;

	if (!strcasecmp("scheduled", code) ||
	    !strcasecmp("3 scheduled", code))
		return SCHEDULED;

	if (!strcasecmp("value change", code) ||
	    !strcasecmp("value_change", code) ||
	    !strcasecmp("4 value change", code))
		return VALUE_CHANGE;

	if (!strcasecmp("connection request", code) ||
	    !strcasecmp("connection_request", code) ||
	    !strcasecmp("6 connection request", code))
		return CONNECTION_REQUEST;

	return BOOTSTRAP;
}

char * lfc_get_current_time(char *format)
{
	time_t time_c;
	struct tm *tm_c;

	time_c = time(NULL);
	tm_c = localtime(&time_c);
	if (tm_c == NULL) return NULL;

	static char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);

	if (strftime(buf, sizeof(buf), format, tm_c) == 0) return NULL;

	return buf;
}

int lfc_get_remaining_time(char *format, char *time_arg)
{
	time_t time_c, time_f;
	struct tm tm_f;

	memset(&tm_f, 0, sizeof(struct tm));
	strptime(time_arg, format, &tm_f);

	time_f = mktime(&tm_f);
	time_c = time(NULL);

	return (int) difftime(time_f, time_c);
}

