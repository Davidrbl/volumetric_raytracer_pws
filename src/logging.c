#include <stdarg.h>
#include <time.h>

#include <logging.h>

static u32 ll = 0;
static FILE *fp = NULL;

void tlog_init(u32 loglevel, FILE *dest) {
    ll = loglevel;
    fp = dest ? dest : stderr;
}

void tlog(u32 loglevel, const char *restrict fmt, ...) {
    if (loglevel < ll) return;
    char t[64] = {'\0'};
    time_t cur = time(NULL);
    strftime(t, sizeof t, "%T", localtime(&cur));
    fprintf(fp, "[%u %s] ", loglevel, t);
    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);
}
