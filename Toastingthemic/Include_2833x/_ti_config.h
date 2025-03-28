/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2001 Mike Barcroft <mike@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SYS_STDINT_H_
#define _SYS_STDINT_H_

#include <_ti_config.h>
#include <sys/cdefs.h>
#include <sys/_types.h>
#include <machine/_stdint.h>

/* Definiciones de tipos enteros estándar */
typedef __int_least8_t    int_least8_t;
typedef __int_least16_t   int_least16_t;
typedef __int_least32_t   int_least32_t;
typedef __int_least64_t   int_least64_t;

typedef __uint_least8_t   uint_least8_t;
typedef __uint_least16_t  uint_least16_t;
typedef __uint_least32_t  uint_least32_t;
typedef __uint_least64_t  uint_least64_t;

typedef __int_fast8_t     int_fast8_t;
typedef __int_fast16_t    int_fast16_t;
typedef __int_fast32_t    int_fast32_t;
typedef __int_fast64_t    int_fast64_t;

typedef __uint_fast8_t    uint_fast8_t;
typedef __uint_fast16_t   uint_fast16_t;
typedef __uint_fast32_t   uint_fast32_t;
typedef __uint_fast64_t   uint_fast64_t;

/* Definición del tamaño de palabra del sistema */
_TI_PROPRIETARY_PRAGMA("diag_push")
_TI_PROPRIETARY_PRAGMA("CHECK_MISRA(\"-10.1\")")
#if defined(UINTPTR_MAX) && defined(UINT64_MAX) && (UINTPTR_MAX == UINT64_MAX)
    #define __WORDSIZE 64
#else
    #define __WORDSIZE 32
#endif
_TI_PROPRIETARY_PRAGMA("diag_pop")

/* Definiciones de wchar_t */
_TI_PROPRIETARY_PRAGMA("diag_push")
_TI_PROPRIETARY_PRAGMA("CHECK_MISRA(\"-19.4\")")
#define WCHAR_MIN  __WCHAR_MIN
#define WCHAR_MAX  __WCHAR_MAX
_TI_PROPRIETARY_PRAGMA("diag_pop")

#if __EXT1_VISIBLE
/* ISO/IEC 9899:2011 K.3.4.4 */
#ifndef RSIZE_MAX
#define RSIZE_MAX (SIZE_MAX >> 1)
#endif
#endif /* __EXT1_VISIBLE */

#endif /* !_SYS_STDINT_H_ */
