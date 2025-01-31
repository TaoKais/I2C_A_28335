#ifndef _STDINT_H_
#define _STDINT_H_

/* Exact-width integer types */
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef signed short        int16_t;
typedef unsigned short      uint16_t;
typedef signed long         int32_t;
typedef unsigned long       uint32_t;
typedef signed long long    int64_t;
typedef unsigned long long  uint64_t;

/* Minimum-width integer types */
typedef int8_t              int_least8_t;
typedef uint8_t             uint_least8_t;
typedef int16_t             int_least16_t;
typedef uint16_t            uint_least16_t;
typedef int32_t             int_least32_t;
typedef uint32_t            uint_least32_t;
typedef int64_t             int_least64_t;
typedef uint64_t            uint_least64_t;

/* Fastest minimum-width integer types */
typedef int8_t              int_fast8_t;
typedef uint8_t             uint_fast8_t;
typedef int16_t             int_fast16_t;
typedef uint16_t            uint_fast16_t;
typedef int32_t             int_fast32_t;
typedef uint32_t            uint_fast32_t;
typedef int64_t             int_fast64_t;
typedef uint64_t            uint_fast64_t;

/* Pointer types */
typedef long int            intptr_t;
typedef unsigned long int   uintptr_t;

/* Greatest-width integer types */
typedef long long           intmax_t;
typedef unsigned long long  uintmax_t;

#endif /* _STDINT_H_ */
