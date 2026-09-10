#ifndef SAFE_C_H_
#define SAFE_C_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>

// #define SAFE_C_LOG_EN__DBG   1
// #define SAFE_C_LOG_EN__INF   1
// #define SAFE_C_LOG_EN__WRN   1
// #define SAFE_C_LOG_EN__ERR   1

enum {
	SAFE_C__VPRINTF_BUF = 256,
};

#if defined(__MINGW32__) || defined(WIN32) || defined(__WIN32)
#define SLASH   '\\'
#else
#define SLASH '/'
#endif

#ifndef __LINE__
#error "__LINE__ is undefined, but must be."
#endif

#ifndef __FILE__
#error "__FILE__ is undefined, but must be."
#endif

#ifdef __ZEPHYR__
#include <zephyr/logging/log.h>
#define LOG_DBGf LOG_DBG
#define LOG_INFf LOG_INF
#define LOG_WRNf LOG_WRN
#define LOG_ERRf LOG_ERR
#define SAFE_C__PANIC() k_panic()
#else
#define _SAFE_C__CUSTOM_PRINT
#define LOG_WITH_PREFIX(prefix, fmt) do { safe_c__printf(prefix " %s:%i: " fmt "\n", safe_c__filename_from_path(__FILE__), __LINE__); } while (0)
#define LOG_MOCK(fmt) do {} while (0)
#define LOGf_WITH_PREFIX(prefix, fmt, ...) do { safe_c__printf(prefix " %s:%i: " fmt "\n", safe_c__filename_from_path(__FILE__), __LINE__, __VA_ARGS__); } while (0)

#ifdef SAFE_C_LOG_EN__DBG
    #define LOG_DBG(fmt) LOG_WITH_PREFIX("[DBG]", fmt)
    #define LOG_DBGf(fmt, ...) LOGf_WITH_PREFIX("[DBG]", fmt, __VA_ARGS__)
#else
    #define LOG_DBG(fmt) LOG_MOCK(fmt)
    #define LOG_DBGf(fmt, ...) LOG_MOCK(fmt)
#endif

#ifdef SAFE_C_LOG_EN__INF
    #define LOG_INF(fmt) LOG_WITH_PREFIX("[INF]", fmt)
    #define LOG_INFf(fmt, ...) LOGf_WITH_PREFIX("[INF]", fmt, __VA_ARGS__)
#else
    #define LOG_INF(fmt) LOG_MOCK(fmt)
    #define LOG_INFf(fmt, ...) LOG_MOCK(fmt)
#endif

#ifdef SAFE_C_LOG_EN__WRN
    #define LOG_WRN(fmt) LOG_WITH_PREFIX("[WRN]", fmt)
    #define LOG_WRNf(fmt, ...) LOGf_WITH_PREFIX("[WRN]", fmt, __VA_ARGS__)
#else
    #define LOG_WRN(fmt) LOG_MOCK(fmt)
    #define LOG_WRNf(fmt, ...) LOG_MOCK(fmt)
#endif

#ifdef SAFE_C_LOG_EN__ERR
    #define LOG_ERR(fmt) LOG_WITH_PREFIX("[ERR]", fmt)
    #define LOG_ERRf(fmt, ...) LOGf_WITH_PREFIX("[ERR]", fmt, __VA_ARGS__)
#else
    #define LOG_ERR(fmt) LOG_MOCK(fmt)
    #define LOG_ERRf(fmt, ...) LOG_MOCK(fmt)
#endif
#endif

// Call func, if error - print err msg, save result in rc and goto finally.
#define TRY(func_expr)       do {int _result = func_expr; if (0 != _result) {LOG_ERRf("Fail to call " #func_expr ": %i", _result); rc = _result; goto finally;}} while (0)

// TRY with error replace.
#define TRYr(func_expr, err) do {int _result = func_expr; if (0 != _result) {LOG_ERRf("Fail to call " #func_expr ": %i", _result); rc = _result; goto finally;}} while (0)

// TRY with allow positive return codes.
#define TRYp(func_expr)       do {int _result = func_expr; if (_result < 0) {LOG_ERRf("Fail to call " #func_expr ": %i", _result); rc = _result; goto finally;}} while (0)

// TRY without save result in rc.
#define TRYv(func_expr) do {int _result = func_expr; if (0 != _result) {LOG_ERRf("Fail to call " #func_expr ": %i", _result); goto finally;}} while (0)

// TRY with do panic instead of goto finally.
#define TRY_PANIC(func_expr)  do {int _result = func_expr; if (0 != _result) {LOG_ERRf("Fail to call " #func_expr ": %i", _result); SAFE_C__PANIC();}} while (0)

// TRY without goto finally if error, but with warning msg. NOT save result to rc.
#define TRY_PASS(func_expr) do {int _result = func_expr; if (0 != _result) {LOG_WRNf("Fail to call " #func_expr ": %i", _result);}} while (0)

// TRY_PASS with debug level msg. NOT save result to rc.
#define TRYs_PASS(func_expr) do {int _result = func_expr; if (0 != _result) {LOG_DBGf("Fail to call " #func_expr ": %i", _result);}} while (0)

// TRY_PASS with format msg. NOT save result to rc.
#define TRYf_PASS(func_expr, fmt, ...) do {int _result = func_expr; if (0 != _result) {LOG_WRNf("Fail to call " #func_expr ": %i, " fmt "", _result, __VA_ARGS__);}} while (0)

// TRY without goto finally if error, but with warning msg. Error checking as in TRYp. NOT save result to rc.
#define TRYp_PASS(func_expr) do {int _result = func_expr; if (_result < 0) {LOG_WRNf("Fail to call " #func_expr ": %i", _result);}} while (0)

// TRY_PASS with save result to rc.
#define TRY_PASS_EX(func_expr) do {int _result = func_expr; if (0 != _result) {LOG_WRNf("Fail to call " #func_expr ": %i", _result);}rc = _result;} while (0)

// Check input bool_expr, do print err msg, save err to rc and goto finally if error.
#define ASSERT(bool_expr, err) do { if (!(bool_expr)) {LOG_ERR("Assertion '" #bool_expr "' failed."); rc = err; goto finally;} } while (0)

// ASSERT with debug level msg.
#define ASSERTs(bool_expr, err) do { if (!(bool_expr)) {LOG_DBG("Assertion '" #bool_expr "' failed."); rc = err; goto finally;} } while (0)

// ASSERT with msg.
#define ASSERTm(bool_expr, err, msg) do { if (!(bool_expr)) {LOG_ERR("Assertion '" #bool_expr "' failed | " msg "."); rc = err; goto finally;} } while (0)

// ASSERT with format msg.
#define ASSERTf(bool_expr, err, fmt, ...) do { if (!(bool_expr)) {LOG_ERR("Assertion '" #bool_expr "' failed | " fmt ".", __VA_ARGS__); rc = err; goto finally;} } while (0)

// ASSERT without save err to rc.
#define ASSERTv(bool_expr) do { if (!(bool_expr)) {LOG_ERR("Assertion '" #bool_expr "' failed."); goto finally;} } while (0)

#ifndef UNUSED
#define UNUSED(var) (void)var
#endif

/// @brief Common error codes with minus.
/// https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno-base.h
/// https://github.com/torvalds/linux/blob/master/include/uapi/asm-generic/errno.h
enum ErrorCodes {
    ER_NOT_PERM = -1,
    ER_NO_ENT = -2,
    ER_IO = -5,
    ER_BAD_FILE = -9,
	ER_AGAIN = -11,
    ER_NO_MEM = -12,
    ER_ACCESS = -13,
    ER_BAD_ADDR = -14,
    ER_BUSY = -16,
    ER_NO_DEV = -19,
    ER_INVAL = -22,
    ER_ENT_TOO_BIG = -27,
    ER_OUT_OF_RANGE = -34,		
    ER_NOT_IMPL = -38,
    ER_OVERFLOW = -75,
    ER_NO_DATA = -61,
    ER_PROTO = -71,
    ER_INTEGRITY = -74,
    ER_NOT_SUPPORTED = -95,
    ER_TIMEDOUT = -110,	
    ER_ALREADY = -114,

    ER_PROTO_INTERNAL = -500,

    ER_1 = -1000,
    ER_2 = -2000,
    ER_3 = -3000,
    ER_4 = -4000,
    ER_5 = -5000,
    ER_6 = -6000,
    ER_7 = -7000,
    ER_8 = -8000,
    ER_9 = -9000,
    ER_10 = -10000,
    ER_11 = -11000,
    ER_12 = -12000,
    ER_13 = -13000,
    ER_14 = -14000,
    ER_15 = -15000,
    ER_16 = -16000,
    ER_17 = -17000,
    ER_18 = -18000,
    ER_19 = -19000,
    ER_20 = -20000,
    ER_21 = -21000,
    ER_22 = -22000,
    ER_23 = -23000,
    ER_24 = -24000,
    ER_25 = -25000,
    ER_26 = -26000,
    ER_27 = -27000,
    ER_28 = -28000,
    ER_29 = -29000,
    ER_30 = -30000,
    ER_31 = -31000,
    ER_32 = -32000,
    ER_33 = -33000,
    ER_34 = -34000,
    ER_35 = -35000,
    ER_36 = -36000,
    ER_37 = -37000,
    ER_38 = -38000,
    ER_39 = -39000,
    ER_40 = -40000,
};

#ifdef _SAFE_C__CUSTOM_PRINT

void safe_c__init(void (*print_func)(char const* str));

void safe_c__printf(char const* fmt, ...);

char const* safe_c__filename_from_path(char const* file_path);

char const* safe_c__buf_to_str(uint8_t const* buf, uint16_t buf_size);

#endif

#if !__BSD_VISIBLE
size_t strlcpy(char* dst, char const* src, size_t dst_size);
#endif

#endif // SAFE_C_H_
