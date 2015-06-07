/*                               -*- Mode: C -*-
 * sysdep.h --
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Thomas Biskup
 * Created On      : Sun Dec 29 21:52:36 1996
 * Last Modified By: Thomas Biskup
 * Last Modified On: Thu Jan  9 21:01:09 1997
 * Update Count    : 42
 * Status          : Unknown, Use with caution!
 *
 * (C) Copyright 1996, 1997 by Thomas Biskup.
 * All Rights Reserved.
 *
 * This software may be distributed only for educational, research and
 * other non-proft purposes provided that this copyright notice is left
 * intact.  If you derive a new game from these sources you also are
 * required to give credit to Thomas Biskup for creating them in the first
 * place.  These sources must not be distributed for any fees in excess of
 * $3 (as of January, 1997).
 */

#ifndef __SYSDEP__

#define __SYSDEP__

 /*
  * Includes.
  */

#include "config.h"

namespace QHack {
	/*
	 * Global functions.
	 */
	public class SysDep
	{
	public:
		static byte rand_byte(byte);
		static ushort rand_int(ushort);
		static uint rand_long(uint);

		static char getkey(void);

		static void init_rand(void);
		static void init_io();
		static void clean_up_io();
		static void clear_screen();
		static void cursor(byte, byte);
		static void prtchar(byte);
		static void prtstr(char *, ...);
		static void stdprtstr(char *);
		static void update(void);
		static void clear_to_eol(void);
		static void set_color(ConsoleColor);

	private:

	};

}
#endif
