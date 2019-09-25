#ifndef _ERROR_PLP_H
#define _ERROR_PLP_H

#include <stdio.h>
#include <stdarg.h>

/* 以下两个函数都定义在 error_plp.c 中 */
void err_ret(const char *fmt, ...);
void err_exit(const char *fmt, ...);

#endif /* _ERROR_PLP_H */