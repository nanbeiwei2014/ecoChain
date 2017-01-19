/* src/libsecp256k1-config.h.  Generated from libsecp256k1-config.h.in by configure.  */
/* src/libsecp256k1-config.h.in.  Generated from configure.ac by autoheader.  */

#ifndef LIBSECP256K1_CONFIG_H

#define LIBSECP256K1_CONFIG_H


/* Define this symbol to enable x86_64 assembly optimizations */
#define USE_ASM_X86_64 1

/* Define this symbol to use a statically generated ecmult table */
#define USE_ECMULT_STATIC_PRECOMPUTATION 1

/* Define this symbol to use endomorphism optimization */
/* #undef USE_ENDOMORPHISM */

/* Define this symbol to use the FIELD_10X26 implementation */
/* #undef USE_FIELD_10X26 */

/* Define this symbol to use the FIELD_5X52 implementation */
#define USE_FIELD_5X52 1

/* Define this symbol to use the native field inverse implementation */
/* #undef USE_FIELD_INV_BUILTIN */

/* Define this symbol to use the num-based field inverse implementation */
#define USE_FIELD_INV_NUM 1

/* Define this symbol to use the gmp implementation for num */
#define USE_NUM_GMP 1

/* Define this symbol to use no num implementation */
/* #undef USE_NUM_NONE */

/* Define this symbol to use the 4x64 scalar implementation */
#define USE_SCALAR_4X64 1

/* Define this symbol to use the 8x32 scalar implementation */
/* #undef USE_SCALAR_8X32 */

/* Define this symbol to use the native scalar inverse implementation */
/* #undef USE_SCALAR_INV_BUILTIN */

/* Define this symbol to use the num-based scalar inverse implementation */
#define USE_SCALAR_INV_NUM 1

/* Version number of package */
//#define VERSION "0.1"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

#endif /*LIBSECP256K1_CONFIG_H*/
