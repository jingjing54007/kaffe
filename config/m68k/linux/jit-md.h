/*
 * m68k/netbsd1/jit-md.h
 * NetBSD m68k JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Written by Marcello Balduccini <marcy@morgana.usr.dsi.unimi.it>, 1997.
 * NOTE: this file is heavily based on m68k/a-ux/jit-md.h and
 * m68k/openbsd2/jit-md.h, written by Richard Henderson <rth@tamu.edu>, 1997.
 */

#ifndef __m68k_netbsd_jit_md_h
#define __m68k_netbsd_jit_md_h

/**/
/* Include common information. */
/**/
#include "m68k/jit.h"

/**/
/* Define where addresses are returned.  */
/**/

#define M68K_RETURN_REF		REG_d0	/* d0 */

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext *ctx

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	do {								\
		(f).retfp = (uintp)__builtin_frame_address(1);		\
		(f).retpc = (uintp)(c)->sc_pc;				\
	} while (0)

#endif
/*
 * m68k/linux/jit-md.h
 * Linux m68k JIT configuration information.
 *
 * Copyright (c) 1996, 1997, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Same as m68k/netbsd1/jit-md.h
 */

#ifndef __m68k_linux_jit_md_h
#define __m68k_linux_jit_md_h

/**/
/* Include common information. */
/**/
#include "m68k/jit.h"

/**/
/* Define where addresses are returned.  */
/**/

#define M68K_RETURN_REF		REG_d0	/* d0 */

/**/
/* Extra exception handling information. */
/**/
#include <signal.h>

#define	EXCEPTIONPROTO							\
	int sig, int code, struct sigcontext *ctx

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	do {								\
		(f).retfp = (uintp)__builtin_frame_address(1);		\
		(f).retpc = (uintp)(c)->sc_pc;				\
	} while (0)

#endif
