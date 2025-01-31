/*****************************************************************************/
/* _gcc_config.h                                                            */
/*                                                                           */
/* MSP430 GCC Configuration Header                                           */
/*                                                                           */
/* Copyright (c) 2025 OpenAI, Adapted from TI's _ti_config.h                  */
/*                                                                           */
/*****************************************************************************/

#ifndef __GCC_CONFIG_H
#define __GCC_CONFIG_H

/* C++ and C specific definitions */

#if defined(__cplusplus)
/* C++ */
# if (__cplusplus >= 201103L)
 /* C++11 */
#  define _GCC_NORETURN [[noreturn]]
#  define _GCC_NOEXCEPT noexcept
# else
 /* C++98/03 */
#  define _GCC_NORETURN __attribute__((noreturn))
#  define _GCC_NOEXCEPT throw()
# endif
#else
/* C */
# if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
 /* C11 */
#  define _GCC_NORETURN _Noreturn
# else
 /* C89/C99 */
#  define _GCC_NORETURN __attribute__((noreturn))
# endif
# define _GCC_NOEXCEPT
#endif

#if defined(__cplusplus) && (__cplusplus >= 201103L)
# define _GCC_CPP11LIB 1
#endif

#if defined(__cplusplus) && (__cplusplus >= 201402L)
# define _GCC_CPP14LIB 1
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) || \
    defined(_GCC_CPP11LIB)
# define _GCC_C99LIB 1
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) || \
    defined(_GCC_CPP14LIB)
# define _GCC_C11LIB 1
#endif

/* _GCC_NOEXCEPT_CPP14 is defined to noexcept only when compiling for C++14. It
   is intended to be used for functions like abort and atexit that are supposed
   to be declared noexcept only in C++14 mode. */
#ifdef _GCC_CPP14LIB
# define _GCC_NOEXCEPT_CPP14 noexcept
#else
# define _GCC_NOEXCEPT_CPP14
#endif

/* Target-specific definitions for MSP430 */
#include "msp430f2491.h" /* Update this according to your specific MSP430 family */

/* Target MSP430 device-specific defines here (e.g., MSP430G2553) */
#define __MSP430G2553__   /* Make sure you define your exact target device */

/* Additional low-level setup or utility macros if needed */

#endif /* ifndef __GCC_CONFIG_H */
