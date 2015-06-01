/*                               -*- Mode: C -*- 
 * misc.h -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Thomas Biskup
 * Created On      : Mon Dec 30 17:22:02 1996
 * Last Modified By: Thomas Biskup
 * Last Modified On: Thu Jan  9 21:05:55 1997
 * Update Count    : 11
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

#ifndef __MISC__

#define __MISC__

/*
 * Global functions.
 */

namespace QHack {
	public class Misc
	{
	public:
		static int16 dice(char *);
		static uint32 iabs(int32);
		static uint32 imax(int32, int32);
		static uint32 imin(int32, int32);
		static char *string(char *, ...);

		static void you(char *, ...);
		static void message(char *, ...);
		static void clear_messages(void);
		static void get_target(coord, coord, coord *, coord *);
	private:

	};
}


#endif
