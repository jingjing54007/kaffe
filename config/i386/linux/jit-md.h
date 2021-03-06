/*
 * i386/linux/jit-md.h
 * Linux i386 JIT configuration information.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __i386_linux_jit_md_h
#define __i386_linux_jit_md_h

/**/
/* Include common information. */
/**/
#include "i386/jit.h"

#define	EXCEPTIONPROTO SIGNAL_ARGS(sig, ctx)

/* Get the first exception frame from a signal handler */
#define	EXCEPTIONFRAME(f, c)						\
	(f).retbp = (c).ebp;						\
	(f).retpc = (c).eip + 1

#endif
