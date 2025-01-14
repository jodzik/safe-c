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

char const* safe_c__buf_to_str(uint8_t const* const buf, uint16_t const buf_size) {
    static char buf_str[SAFE_C__VPRINTF_BUF] = {0};
    int ptr = 0;

    memset(buf_str, 0, sizeof(buf_str));
    buf_str[0] = '[';
    ptr = 1;
    for (int i = 0; i < buf_size; i++) {
        int const rc = snprintf(&buf_str[ptr], sizeof(buf_str) - ptr, "%02X%s", buf[i],
            buf_size - 1 == i ? "" : " ");
        if (rc <= 0) {
            break;
        }
        ptr += rc;
    }
    if (ptr < sizeof(buf_str) - 1) {
        buf_str[ptr] = ']';
    }

    return buf_str;
}
