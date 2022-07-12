#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#include <standard_types.h>

void tlog_init(u32 loglevel, FILE *dest);
void tlog(u32 loglevel, const char *restrict fmt, ...);

#endif
