#include "error_plp.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef MAXLINE
#undef MAXLINE
#endif

#define MAXLINE 4096

static void err_doit(const char *fmt, va_list ap);
void err_ret(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(fmt, ap);
    va_end(ap);
}
void err_exit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    err_doit(fmt, ap);
    va_end(ap);
    exit(1);
}

static void err_doit(const char *fmt, va_list ap)
{
    char buf[MAXLINE];
    int ret;
    ret = vsnprintf(buf, MAXLINE, fmt, ap);
    if (ret < 0)
    {
        return;
    }
    fflush(stdout);
    fputs(buf, stderr);
    fflush(NULL); /* necessary? 估计在重定向里面有用 */
}