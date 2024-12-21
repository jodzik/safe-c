#include "safe_c.h"

#include <stdlib.h>
#include <stdarg.h>

void (*g_safe_c_print)(char const* str) = NULL;

void safe_c__init(void (*print_func)(char const* str)) {
    g_safe_c_print = print_func;
}

void safe_c__printf(char const* const fmt, ...) {
	int rc = 0;
    static char buf[SAFE_C__VPRINTF_BUF] = {0};
	va_list arg_list = {0};
    va_start(arg_list, fmt);
    rc = vsnprintf(buf, sizeof(buf), fmt, arg_list);
    va_end(arg_list);
    if (g_safe_c_print && rc > 0) {
    	g_safe_c_print(buf);
    }
}

char const* safe_c__filename_from_path(char const* const file_path) {
	return (strrchr(file_path, SLASH) ? strrchr(file_path, SLASH) + 1 : file_path);
}
